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

#include "ServerImpl.h"

#include <atomic>
#include <vector>
#include <map>

class Connection;


/**
 * Network server for sending logs.
 */

class NetLogger : public ServerImpl {
public:
 /**
  * Class constructor
  * \param msg_queue_size amount of last logs which will be stored
  */
  NetLogger(size_t msg_queue_size);
  /**
   * Overrides ServerImpl
   */
  void OnClientConnected(std::shared_ptr<Client> client) override;
  /**
   * Overrides ServerImpl
   */
  void OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) override;

  /**
   * Adds new logs to store and send to connected clients
   * msg log message
   */
  void Log(const std::string& msg);

protected :
  /**
   * Stores a messege for future connectd clinets
   * \param msg network message object containig log
   */
  void AddMsg(std::shared_ptr<Message> msg);
   /**
   * Sends all stored messages
   * \param client which will receive messages
   */ 
  void SendMsgs(std::shared_ptr<Client> client);
  std::vector<std::shared_ptr<Message> > _messages; ///< stored messages collection
  std::mutex _msgs_mutex; ///< messages collection access mutex
  size_t _msg_queue_size; ///< max amount of stored messages
};
