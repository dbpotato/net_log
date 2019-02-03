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
  - **nread** - network client

# Integration

You should be able to integrate NetLog with your existing code
by editing one  of your source files. Take a look at example.cpp

What you need is just NetLogWrapper.h copied into one of your include directories.

You can configure it by changing default const variables:
 - **NET_LOG_SERVER_PORT**
 - **NET_LOG_MSG_QUEUE_SIZE**
 - **NET_LOG_MSG_FORMAT**

# Usage

Copy **libnetlog.so** to your other libraries or set LD_LIBRARY_PATH environment variable.
In your code use **netlog** same way as you would use **printf** or **ostringstream**.
For exmple use : **netlog("the answer is %d", 42)** instead of **printf("the answer is %d", 42)**
or use use : **netlog() << "the answer is " << 42** instead of **cout << "the answer is " << 42**

Receive generated logs with build **nread** client followed by HOST and PORT arguments.

# License

MIT