/*
Copyright (c) 2018 - 2019 Adam Kaniewski

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
#include "NetLoggerServer.h"
#include "NetLoggerClient.h"
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

NetLogger::NetLogger(bool is_sender, size_t msg_queue_size, std::string log_format)
    : _is_sender(is_sender)
    , _msg_queue_size(msg_queue_size)
    , _log_format(log_format) {
}

bool NetLogger::Init(int port, std::string host) {
  Logger::Instance().SetLogFunc(spdlog_func, MSG_LOGGER_ID, this);
  log(MSG_LOGGER_ID)->set_pattern(_log_format);

  _connection = std::make_shared<Connection>();
  _connection->Init();

  if(host.empty()) {
    _server = std::make_shared<NetLoggerServer>(shared_from_this());
    return _server->Init(port, _connection);
  }

  _client = std::make_shared<NetLoggerClient>(_connection,
                                              shared_from_this(),
                                              port,
                                              host,
                                              _is_sender);
  _client->Init();

  return true;
}

void NetLogger::GetMsgs(std::vector<std::shared_ptr<Message> >& out_messages) {
  std::lock_guard<std::mutex> lock(_msgs_mutex);
  out_messages = _messages;
}

void NetLogger::Log(const std::string& msg) {
  if(_is_sender)
    log(MSG_LOGGER_ID)->info(msg);
  else
    log()->info("{}", msg);
}

void NetLogger::LogInternal(const std::string& msg) {
  if(!_is_sender)
    return;

  auto msg_obj = std::make_shared<Message>((uint8_t)MessageType::YOU_SHOULD_KNOW_THAT, msg);
  AddMsg(msg_obj);
  if(_server)
    _server->SendLog(msg_obj);
  else if(_client)
    _client->SendLog(msg_obj);
}

void NetLogger::AddMsg(std::shared_ptr<Message> msg) {
  if(!_is_sender)
    return;

  std::lock_guard<std::mutex> lock(_msgs_mutex);
  if(_messages.size() && (_messages.size() == _msg_queue_size))
    _messages.erase(_messages.begin());
  _messages.push_back(msg);
}

