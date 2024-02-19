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
#include "DataResource.h"
#include "SimpleMessage.h"
#include "MessageType.h"
#include "Logger.h"


std::shared_ptr<NetLoggerClient> NetLoggerClient::Create(
                  std::shared_ptr<Connection> connection,
                  std::shared_ptr<NetLogger> owner,
                  int port,
                  const std::string& host,
                  bool is_sender) {
  std::shared_ptr<NetLoggerClient> netlogger_client;
  netlogger_client.reset(new NetLoggerClient(connection, owner, is_sender));
  netlogger_client->Init(host, port);
  return netlogger_client;
}

NetLoggerClient::NetLoggerClient(std::shared_ptr<Connection> connection,
                                 std::shared_ptr<NetLogger> owner,
                                 bool is_sender)
    : _owner(owner)
    , _is_sender(is_sender)
    , _connection(connection) {
}

void NetLoggerClient::Init(const std::string& host, int port) {
  ConnectionChecker::MointorUrl(host, port, shared_from_this());
}

bool NetLoggerClient::OnClientConnecting(std::shared_ptr<Client> client, NetError err) {
  if(err != NetError::OK) {
    return false;
  }

  auto msg_builder = std::unique_ptr<SimpleMessageBuilder>(new SimpleMessageBuilder());
  client->SetMsgBuilder(std::move(msg_builder));
  return true;
}

void NetLoggerClient::OnClientConnected(std::shared_ptr<Client> client) {
  std::vector<std::shared_ptr<SimpleMessage>> messages;
  _client = client;
  _owner->GetMsgs(messages);
  for(auto& msg : messages) {
    client->Send(msg);
  }
}

void NetLoggerClient::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  std::shared_ptr<SimpleMessage> simple_msg = std::static_pointer_cast<SimpleMessage>(msg);
  auto msg_header = simple_msg->GetHeader();
  auto msg_content = simple_msg->GetContent();
  auto msg_data = msg_content->GetMemCache();

  if(!msg_content->IsCompleted()) {
    return;
  }

  switch(MessageType::TypeFromInt(msg_header->_type)) {
    case MessageType::YOU_SHOULD_KNOW_THAT:
      _owner->Log(msg_data->ToString());
      break;
    default:
      break;
  }
}

void NetLoggerClient::OnClientClosed(std::shared_ptr<Client> client) {
  if(!_is_sender) {
    log()->info("NetReader: Connection closed, awaiting new connection");
  }
}

void NetLoggerClient::CreateClient(std::shared_ptr<MonitorTask> task, const std::string& url, int port) {
  _connection->CreateClient(port, url, task);
}

void NetLoggerClient::SendPingToClient(std::shared_ptr<Client> client) {
  client->Send(std::make_shared<SimpleMessage>((uint8_t)MessageType::ARE_U_ALIVE));
}

void NetLoggerClient::SendLog(std::shared_ptr<Message> msg) {
  if(_client) {
    _client->Send(msg);
  }
}
