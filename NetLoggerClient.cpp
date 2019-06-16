/*
Copyright (c) 2019 Adam Kaniewski

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "NetLoggerClient.h"
#include "NetLogger.h"
#include "Message.h"
#include "MessageType.h"
#include "Logger.h"

const size_t CHECK_INTERVAL_IN_SEC = 3;

NetLoggerClient::NetLoggerClient(std::shared_ptr<NetLogger> owner, bool is_sender)
    : _owner(owner)
    , _is_sender(is_sender) {
}

void NetLoggerClient::Init(std::shared_ptr<Connection> connection,
                                      int port,
                                      std::string host) {
    _checker = std::make_shared<ConnectionChecker>(shared_from_this()
                                                ,connection
                                                ,CHECK_INTERVAL_IN_SEC
                                                ,host
                                                ,port);
    _checker->Init();
}

void NetLoggerClient::OnConnected(std::shared_ptr<Client> client) {
  std::lock_guard<std::mutex> lock(_client_mutex);
  _client = client;
  _client->Start(shared_from_this());

  std::vector<std::shared_ptr<Message> > messages;
  _owner->GetMsgs(messages);
  for(auto msg : messages)
    _client->Send(msg);
}

void NetLoggerClient::OnDisconnected() {
  CloseClient();
}

void NetLoggerClient::SendPing() {
  std::lock_guard<std::mutex> lock(_client_mutex);
  if(_client)
    _client->Send(std::make_shared<Message>((uint8_t)MessageType::ARE_U_ALIVE));

}

void NetLoggerClient::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  _checker->Wake();
  switch(MessageType::TypeFromInt(msg->_type)) {
    case MessageType::YOU_SHOULD_KNOW_THAT:
      _owner->Log(msg->ToString());
      break;
    default:
      break;
  }
}

void NetLoggerClient::SendLog(std::shared_ptr<Message> msg) {
  std::lock_guard<std::mutex> lock(_client_mutex);
  if(_client)
    _client->Send(msg);
}

void NetLoggerClient::OnClientClosed(std::shared_ptr<Client> client) {
  CloseClient();
}

void NetLoggerClient::CloseClient(){
  std::lock_guard<std::mutex> lock(_client_mutex);
  if(_client) {
    if(!_is_sender)
      log()->info("NetReader: Connection closed, awaiting new connection");
    _client.reset();
  }
  _checker->Reset();
}