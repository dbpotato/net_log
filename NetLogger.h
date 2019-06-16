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

#pragma once


#include <atomic>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

class Connection;
class NetLoggerServer;
class NetLoggerClient;
class Client;
class Message;


/**
 * Logs Sender / Reader
 */

class NetLogger : public std::enable_shared_from_this<NetLogger> {
public:
 /**
  * Class constructor
  * \param is_sender if true NetLogger will act as sender. Acts as a reader if false
  * \param msg_queue_size amount of last logs which will be stored
  * \param log_format spdlog format which will be added to messages
  */
  NetLogger(bool is_sender, size_t msg_queue_size = 0, std::string log_format = {});

  /**
   * Initialize network operations
   * \param port network port used for connection
   * \param host if empty NetLogger will act as a server, otherwise it will try to connect to it.
   * \return true on success
   */
  bool Init(int port, std::string host);

 /**
  * Called by NetLoggerClient / NetLoggerServer after
  * establishing a connection
  * \param out_messages stored messages
  */
  void GetMsgs(std::vector<std::shared_ptr<Message> >& out_messages);

  /**
   * Passthrough message trough spdlog to add formating
   * \param msg log message
   */
  void Log(const std::string& msg);

  /**
   * Adds new logs to store and send to connected clients
   * \param msg log message
   */
  void LogInternal(const std::string& msg);

protected :
  /**
   * Stores a message for future connected clinets
   * \param msg network message object containing log
   */
  void AddMsg(std::shared_ptr<Message> msg);

  std::shared_ptr<Connection> _connection; ///< connection instance
  std::shared_ptr<NetLoggerServer> _server; ///< sever instance or null if working as a client
  std::shared_ptr<NetLoggerClient> _client; ///< client instance or null if working as a sever
  std::vector<std::shared_ptr<Message> > _messages; ///< stored messages collection
  std::mutex _msgs_mutex; ///< messages collection access mutex
  bool _is_sender; ///< informs if this is a sender or reader
  size_t _msg_queue_size; ///< max amount of stored messages
  std::string _log_format; ///< format used by spdlog
};
