/*
Copyright (c) 2018 Adam Kaniewski

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

#include "NetLogger.h"
#include "Message.h"
#include "MessageType.h"
#include "Connection.h"
#include "Logger.h"

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>

const int MSG_LOGGER_ID = 10;

void spdlog_func(spdlog::level::level_enum lv, const std::string& str, void* arg) {
  auto net_logger = static_cast<NetLogger*>(arg);
  net_logger->LogInternal(str);
}

NetLogger::NetLogger(size_t msg_queue_size, std::string log_format)
    : _msg_queue_size(msg_queue_size) {
  set_log_func(spdlog_func, MSG_LOGGER_ID, this);
  log(MSG_LOGGER_ID)->set_pattern(log_format);
}

void NetLogger::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  switch (MessageType::TypeFromInt(msg->_type)) {
    case MessageType::ARE_U_ALIVE:
      client->Send(std::make_shared<Message>((uint8_t)MessageType::IAM_ALIVE));
      break;
    default:
      break;
  }
}

void NetLogger::OnClientConnected(std::shared_ptr<Client> client) {
  ServerImpl::OnClientConnected(client);
  SendMsgs(client);
}

void NetLogger::Log(const std::string& msg) {
  log(MSG_LOGGER_ID)->info(msg);
}

void NetLogger::LogInternal(const std::string& msg) {
  auto msg_obj = std::make_shared<Message>((uint8_t)MessageType::YOU_SHOULD_KNOW_THAT, msg);
  AddMsg(msg_obj);

  std::vector<std::shared_ptr<Client> > clients;
  GetClients(clients);
  for(auto client : clients)
    client->Send(msg_obj);
}

void NetLogger::AddMsg(std::shared_ptr<Message> msg) {
  std::lock_guard<std::mutex> lock(_msgs_mutex);
  if(_messages.size() && (_messages.size() == _msg_queue_size))
    _messages.erase(_messages.begin());
  _messages.push_back(msg);
}

void NetLogger::SendMsgs(std::shared_ptr<Client> client) {
  std::lock_guard<std::mutex> lock(_msgs_mutex);
  for(auto msg : _messages)
    client->Send(msg);
}
