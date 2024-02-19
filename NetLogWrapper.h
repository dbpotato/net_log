/*
Copyright (c) 2018 - 2024 Adam Kaniewski

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
 * Clang's exit-time destructor workaround
 */
#ifndef NETLOG_DEFINE_STATIC_LOCAL
  #define NETLOG_DEFINE_STATIC_LOCAL(name) static NetLog& name = *new NetLog;
#endif
/*
 * Connection port. It's usage depends on DEFAULT_NET_LOG_HOST value.
 */
static const int DEFAULT_NET_LOG_PORT = 4156;

/*
 * Leave empty to make the netlog creating a server which will listen for connection
 * on NET_LOG_PORT. Provide a hostname and netlog will automatically be
 * connecting / reconnecting with DEFAULT_NET_LOG_HOST:DEFAULT_NET_LOG_PORT.
 */
static const char* DEFAULT_NET_LOG_HOST = "localhost";

/*
* Used to override DEFAULT_NET_LOG_PORT
*/
static const char* ENV_NET_LOG_PORT_VAR_NAME = "NET_LOG_SERVER_PORT";

/*
* Used to override DEFAULT_NET_LOG_HOST
*/
static const char* ENV_NET_LOG_HOST_VAR_NAME = "NET_LOG_SERVER_HOST";

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
      : _init(nullptr)
      , _log_msg(nullptr)
      , _is_valid(false)
      , _lib_handler(nullptr) {

    if(!(_lib_handler = dlopen("libnetlog.so", RTLD_NOW))) {
      printf("NetLog : Can't load libnetlog.so %s\n", dlerror());
    }
    else {
      _init = (int(*)(int, const char*, size_t, const char*)) dlsym(_lib_handler, "init");
      _log_msg = (void(*)(const char*)) dlsym(_lib_handler, "log_msg");

      if(_init && _log_msg) {
        const char* host = DEFAULT_NET_LOG_HOST;
        int port = DEFAULT_NET_LOG_PORT;

        const char* host_env_val = std::getenv(ENV_NET_LOG_HOST_VAR_NAME);

        if(host_env_val){
          host = host_env_val;
        }

        const char* port_env_val = std::getenv(ENV_NET_LOG_PORT_VAR_NAME);
        if(port_env_val) {
          port = atoi(port_env_val);
        }

        if(_init(port,
                host,
                NET_LOG_MSG_QUEUE_SIZE,
                NET_LOG_MSG_FORMAT) < 2) {
          _is_valid = true;
        } else {
          printf("NetLog : Can't start server at port %d\n", port);
        }
      }
      else {
        printf("NetLog : Can't load some functions\n");
      }
    }
  }

  void Log(const std::string& msg)
  {
    if(_is_valid) {
      _log_msg(msg.c_str());
    }
  }

  static NetLog& Instance() {
    NETLOG_DEFINE_STATIC_LOCAL(instance);
    return instance;
  }

private:
  int(*_init)(int, const char*, size_t, const char*);
  void(*_log_msg)(const char*);
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
    std::vector<char> buf((size_t)(1 + vsnprintf(nullptr, 0, msg.c_str(), args1)));
    va_end(args1);
    vsnprintf(buf.data(), buf.size(), msg.c_str(), args2);
    va_end(args2);
    *this << std::string(buf.begin(),buf.end());
  }
  ~netlog() override {
    NetLog::Instance().Log(str());
  }
};

