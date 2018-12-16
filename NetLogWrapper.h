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


#include <dlfcn.h>
#include <stdio.h>
#include <string>
#include <cstdarg>
#include <vector>

const int SERVER_PORT = 4156; ///< listening port

/*
 * Number of stored logs. On connection client
 * will recive all stored logs.
 */
const int MSG_QUEUE_SIZE = 200;

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
    init = (bool(*)(int, size_t)) dlsym(_lib_handler, "init");
    log_msg = (void(*)(const char*)) dlsym(_lib_handler, "log_msg");
 
    if(init && log_msg) {
      if(init(SERVER_PORT, MSG_QUEUE_SIZE))
        _is_valid = true;
      else
        printf("NetLog : Can't start server at port %d\n", SERVER_PORT);
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
  bool(*init)(int, size_t);
  void(*log_msg)(const char*);
  bool _is_valid;
  void* _lib_handler;
};

static void netlog(const std::string& msg, ...) {
  va_list args1;
  va_start(args1, msg);
  va_list args2;
  va_copy(args2, args1);
  std::vector<char> buf(1 + vsnprintf(nullptr, 0, msg.c_str(), args1));
  va_end(args1);
  vsnprintf(buf.data(), buf.size(), msg.c_str(), args2);
  va_end(args2);
  NetLog::Instance().Log(std::string(buf.begin(),buf.end()));
}
