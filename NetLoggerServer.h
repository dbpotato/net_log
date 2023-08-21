/*
Copyright (c) 2019 - 2023 Adam Kaniewski

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

#pragma once

#include "Server.h"

#include <memory>

class NetLogger;

/**
 * Server version implementation of NetLogger
 */
class NetLoggerServer : public ClientManager
                      , public std::enable_shared_from_this<NetLoggerServer>  {
public:
 /**
  * Class constructor
  * \param owner NetLogger parent object
  */
  NetLoggerServer(std::shared_ptr<NetLogger> owner);

 /**
  * Initialize server
  * \param port listening port
  * \param connection connection instance used for sever request
  * \return true if server started successfully
  */
  bool Init(int port, std::shared_ptr<Connection> connection);

  /**
   * Pass messge to connected clients
   * \param msg Message with log
   */
  void SendLog(std::shared_ptr<Message> msg);

  /**
   * Implementes ClientManager interface
   */
  void OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) override;

  /**
   * Implementes ClientManager interface
   */
  bool OnClientConnecting(std::shared_ptr<Client> client, NetError err) override;

  /**
   * Implementes ClientManager interface
   */
  void OnClientConnected(std::shared_ptr<Client> client) override;

private:
  std::shared_ptr<NetLogger> _owner; ///< NetLogger instance
  std::shared_ptr<Server> _server; ///< Server instance
};
