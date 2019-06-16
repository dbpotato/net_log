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

#include "NetLoggerServer.h"
#include "NetLogger.h"
#include "Message.h"
#include "MessageType.h"
#include "Logger.h"

NetLoggerServer::NetLoggerServer(std::shared_ptr<NetLogger> owner)
    : _owner(owner) {
}

void NetLoggerServer::OnClientConnected(std::shared_ptr<Client> client) {
  ServerImpl::OnClientConnected(client);

  std::vector<std::shared_ptr<Message> > messages;
  _owner->GetMsgs(messages);
  for(auto msg : messages)
    client->Send(msg);
}

void NetLoggerServer::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  switch (MessageType::TypeFromInt(msg->_type)) {
    case MessageType::ARE_U_ALIVE:
      client->Send(std::make_shared<Message>((uint8_t)MessageType::IAM_ALIVE));
      break;
    case MessageType::YOU_SHOULD_KNOW_THAT:
      _owner->Log(msg->ToString());
      break;
    default:
      break;
  }
}

void NetLoggerServer::SendLog(std::shared_ptr<Message> msg) {
  std::vector<std::shared_ptr<Client> > clients;
  GetClients(clients);
  for(auto client : clients)
    client->Send(msg);
}
