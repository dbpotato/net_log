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

#pragma once

#include "Client.h"
#include "ConnectionChecker.h"

class NetLogger;

/**
 * Client version implementation of NetLogger
 */
class NetLoggerClient : public std::enable_shared_from_this<NetLoggerClient>
               , public ClientManager
               , public ConnectionKeeper {
public:
 /**
  * Class constructor
  * \param owner NetLogger parent object
  * \param is_sender informs if it will work as reader or sender
  */
  NetLoggerClient(std::shared_ptr<NetLogger> owner, bool is_sender);


 /**
  * Initialize and starts ConnectionKeeper
  * \param connection Connection instance
  * \param port host's port
  * \param host host's address
  */
  void Init(std::shared_ptr<Connection> connection, int port, std::string host);

  /**
   * Implementes ConnectionKeeper interface
   */
  void OnConnected(std::shared_ptr<Client> client) override;

  /**
   * Implementes ConnectionKeeper interface
   */
  void OnDisconnected() override;

  /**
   * Implementes ConnectionKeeper interface
   */
  void SendPing() override;

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

  /**
   * Implementes ClientManager interface
   */
  void OnMsgSent(std::shared_ptr<Client> client, std::shared_ptr<Message> msg, bool success){;}
private:

  /**
   * Relese Client object after disconnection
   */
  void CloseClient();

  std::shared_ptr<NetLogger> _owner; ///< NetLogger instance
  std::shared_ptr<ConnectionChecker> _checker; ///< checker instance
  std::shared_ptr<Client> _client; ///< connected Client
  std::mutex _client_mutex; ///< client access mutex
  bool _is_sender; ///<informs if this is a sender or reader
};
