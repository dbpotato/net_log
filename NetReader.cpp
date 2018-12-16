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

#include "NetReader.h"
#include "Logger.h"
#include "MessageType.h"

#include <unistd.h>
#include <cstdlib>

const size_t CHECK_INTERVAL_IN_SEC = 3;

NetReader::NetReader() {
  _connection = std::make_shared<Connection>();
  _connection->Init();
  log()->set_pattern("%v");
}

void NetReader::Init(const std::string& host, int port) {
  if(!_checker) {
    _checker = std::make_shared<ConnectionChecker>(shared_from_this()
                                                 ,_connection
                                                ,CHECK_INTERVAL_IN_SEC
                                                ,host
                                                ,port);
    _checker->Init();

    log()->info("NetReader: Awaiting connection");
  }
  else
    log()->error("NetReader: Already initialized");
}


void NetReader::OnConnected(std::shared_ptr<Client> client) {
  log()->info("NetReader: Connected");
  std::lock_guard<std::mutex> lock(_client_mutex);
  _client = client;
  _client->Start(shared_from_this());
}

void NetReader::OnDisconnected() {
  CloseClient();
}

void NetReader::SendPing() {
  std::lock_guard<std::mutex> lock(_client_mutex);
  if(_client)
    _client->Send(std::make_shared<Message>((uint8_t)MessageType::ARE_U_ALIVE));
}

void NetReader::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  _checker->Wake();
  switch(MessageType::TypeFromInt(msg->_type)) {
    case MessageType::YOU_SHOULD_KNOW_THAT:
      OnLogReceived(msg->ToString());
      break;
    default:
      break;
  }
}

void NetReader::OnLogReceived(const std::string& msg) {
  //log()->info("{}", msg);
  printf("%s\n", msg.c_str());
}

void NetReader::OnClientClosed(std::shared_ptr<Client> client) {
  CloseClient();
}

void NetReader::CloseClient(){
  std::lock_guard<std::mutex> lock(_client_mutex);
  if(_client) {
    log()->info("NetReader: Connection closed, awaiting new connection");
    _client.reset();
  }
}

int main(int argc,const char** args) {
  if(argc < 3)
      return 0;
  std::shared_ptr<NetReader> reader = std::make_shared<NetReader>();
  reader->Init(args[1], std::atoi(args[2]));
  while(true)
    sleep(1);
  return 0;
}

