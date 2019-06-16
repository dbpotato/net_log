# NetLog Library

NetLog is **c++ Linux library** which basically provides printf over a network.
It's easy to integrate into your existing code.

# Building

Uses cmake. If you want to cross compile then export full name of the
compiler under **CXX** env variable. By default **g++** is used.
Project contains recursive submodules, init them with: **git submodule update --init --recursive**
Output files will be placed in "bin_compiler_name" directory:
- **libnetlog.so** - library
- **example** - basic integration example
- **nread** - network log reader

# Integration

To integrate NetLog with your existing code just incude **NetLogWrapper.h**
in your source files. Take a look at example.cpp

You can configure it by changing default const variables:
- **NET_LOG_PORT**
- **NET_LOG_HOST**
- **NET_LOG_MSG_QUEUE_SIZE**
- **NET_LOG_MSG_FORMAT**

# Usage

Copy **libnetlog.so** to your other libraries or set LD_LIBRARY_PATH environment variable.
In your code use **netlog** same way as you would use **printf** or **ostringstream**.
For example use : **netlog("the answer is %d", 42)** instead of **printf("the answer is %d", 42)**
or use : **netlog() << "the answer is " << 42** instead of **cout << "the answer is " << 42**

Netlog instance can work as a network client or server depending on NetLogWrapper.h

nread - Netlog's reader will work as a network client or server depending on passed parameters.
For example use : **nread 4156** to start reader as a server awaiting connections from NetLog instances,
use : **nread localhost 4156** to start reader as a client trying to connect to NetLog instance.

# License

MIT
