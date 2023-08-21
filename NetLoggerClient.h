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

#include "Client.h"
#include "ConnectionChecker.h"

class NetLogger;


/**
 * Client version implementation of NetLogger
 */
class NetLoggerClient : public ConnectionChecker {
public:
 /**
  * Class constructor
  * \param connection Connection instance
  * \param owner NetLogger parent object
  * \param port host's port
  * \param host host's address
  * \param is_sender informs if it will work as reader or sender
  */
  NetLoggerClient(std::shared_ptr<Connection> connection,
                  std::shared_ptr<NetLogger> owner,
                  int port,
                  const std::string& host,
                  bool is_sender);

  /**
   * Implementes ClientManager interface
   */
  bool OnClientConnecting(std::shared_ptr<Client> client, NetError err) override;

  /**
   * Implementes ConnectionKeeper interface
   */
  void OnClientConnected(std::shared_ptr<Client> client) override;

  /**
   * Implementes ConnectionKeeper interface
   */
  std::shared_ptr<Message> CreatePingMessage() override;

  /**
   * Passes log to connected client
   * \param msg message object containing log
   */
  void SendLog(std::shared_ptr<Message> msg);

  /**
   * Implementes ClientManager interface
   */
  void OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) override;

  /**
   * Implementes ClientManager interface
   */
  void OnClientClosed(std::shared_ptr<Client> client) override;


private:
  std::shared_ptr<NetLogger> _owner; ///< NetLogger instance
  bool _is_sender; ///<informs if this is a sender or reader
};
