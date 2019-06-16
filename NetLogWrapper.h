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

#include <dlfcn.h>
#include <stdio.h>
#include <string>
#include <cstdarg>
#include <vector>
#include <sstream>


/*
 * Connection port. It's usage depends on NET_LOG_HOST value.
 */
static const int NET_LOG_PORT = 4156;

/*
 * Leave empty to make the netlog creating a server which will listen for connection
 * on NET_LOG_PORT. Provide a hostname and netlog will automatically be
 * connecting / reconnecting with NET_LOG_HOST:NET_LOG_PORT.
 */
static const char* NET_LOG_HOST = "localhost";

/*
 * Number of stored logs. On connection readers
 * will recive all stored logs.
 */
static const int NET_LOG_MSG_QUEUE_SIZE = 200;

/*
 * Message format as described at :
 * https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
 * For example '[%H:%M:%S:%e] [%t] %v' will provide additional time and thread info.
 * By defaul nothing is added to original message.
 */
static const char* NET_LOG_MSG_FORMAT="%v";

class NetLog {
public:
  NetLog()
      : init(nullptr)
      , log_msg(nullptr)
      , _is_valid(false)
      , _lib_handler(nullptr) {

    if(!(_lib_handler = dlopen("libnetlog.so", RTLD_NOW))) {
      printf("NetLog : Can't load libnetlog.so %s\n", dlerror());
    }
    else {
      init = (bool(*)(int, const char*, size_t, const char*)) dlsym(_lib_handler, "init");
      log_msg = (void(*)(const char*)) dlsym(_lib_handler, "log_msg");

      if(init && log_msg) {
        if(init(NET_LOG_PORT,
                NET_LOG_HOST,
                NET_LOG_MSG_QUEUE_SIZE,
                NET_LOG_MSG_FORMAT))
          _is_valid = true;
        else
          printf("NetLog : Can't start server at port %d\n", NET_LOG_PORT);
      }
      else
        printf("NetLog : Can't load some functions\n");
    }
  }

  ~NetLog() {
    if(_lib_handler)
      dlclose(_lib_handler);
  }

  void Log(const std::string& msg)
  {
    if(_is_valid) {
      log_msg(msg.c_str());
    }
  }
  static NetLog& Instance() {
    static NetLog instance;
    return instance;
  }
protected:
  bool(*init)(int, const char*, size_t, const char*);
  void(*log_msg)(const char*);
  bool _is_valid;
  void* _lib_handler;
};

class netlog : public std::ostringstream {
public:
  netlog(){}
  netlog(const std::string msg, ...) {
    va_list args1;
    va_start(args1, msg);
    va_list args2;
    va_copy(args2, args1);
    std::vector<char> buf(1 + vsnprintf(nullptr, 0, msg.c_str(), args1));
    va_end(args1);
    vsnprintf(buf.data(), buf.size(), msg.c_str(), args2);
    va_end(args2);
    *this << std::string(buf.begin(),buf.end());
  }
  virtual ~netlog() {
    NetLog::Instance().Log(str());
  }
};
