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

#pragma once

#include "Message.h"
#include "Client.h"
#include "ConnectionChecker.h"

#include <memory>

/**
 * Network client for receiving logs.
 * Once starded it connectsts to / monitors server
 * trough ConnectionChecker.
 */
class NetReader : public std::enable_shared_from_this<NetReader>
                , public ClientManager
                , public ConnectionKeeper {
public:
  NetReader();
  /**
   * Starts ConnectionChecker
   * \param host address of netloger server
   * \param port port of netloger server
   */
  void Init(const std::string& host, int port);

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
  void OnMsgSent(std::shared_ptr<Client> client, std::shared_ptr<Message> msg, bool success) override{};
private :
  /**
   * Prints received message
   * \param masg message to print
   */  
  void OnLogReceived(const std::string& msg);

  /**
   * Relese Client object after disconnection
   */  
  void CloseClient();

  std::shared_ptr<ConnectionChecker> _checker; ///< checker instance
  std::shared_ptr<Connection> _connection; ///< connection instance
  std::shared_ptr<Client> _client; ///< client instance
  std::mutex _client_mutex; ///< mutex for client modifications
};
