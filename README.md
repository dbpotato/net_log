# NetLog Library

NetLog is **c++ Linux library** which basically provides printf over a network.
It's easy to integrate into your existing code.

# Building

Uses cmake. If you want to cross compile pass export full name of the
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
 - **SERVER_PORT**
 - **MSG_QUEUE_SIZE**

# Usage

Copy **libnetlog.so** to your other libraries or set LD_LIBRARY_PATH environment variable.
In your code use **netlog** function same way as you would use **printf**.
Receive generate logs with build **nread** used with HOST and PORT arguments.

# License

MIT