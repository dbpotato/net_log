#[[
Copyright (c) 2018 - 2023 Adam Kaniewski

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
]]

cmake_minimum_required(VERSION 3.2)
project(NetLog)

set(CXX_FLAGS
 "-O2 \
 -std=gnu++17 \
 -Wall \
 -Werror \
 -Wundef \
 -Wcast-qual \
 -Wno-unused \
 -Wno-delete-non-virtual-dtor"
)

set(LD_FLAGS
 "-lpthread \
  -ldl"
)

#add_definitions(-DENABLE_DEBUG_LOGGER)

set(CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_SYSTEM_NAME linux)
set(DEFAULT_CXX "g++")

set(COMMON_DIR "${PROJECT_SOURCE_DIR}/common")
set(SRC_DIR "${PROJECT_SOURCE_DIR}")


if(DEFINED ENV{CUSTOM_CXX})
  message("Using user's compiler : " $ENV{CUSTOM_CXX})
  set(CMAKE_CXX_COMPILER $ENV{CUSTOM_CXX})
else(DEFINED ENV{CUSTOM_CXX})
  message("Using default compiler : " ${DEFAULT_CXX})
  set(CMAKE_CXX_COMPILER ${DEFAULT_CXX})
endif(DEFINED ENV{CUSTOM_CXX})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER})


include_directories(
  ${COMMON_DIR}/third_party/spdlog/include
  ${COMMON_DIR}/tools/thread
  ${COMMON_DIR}/tools/logger
  ${COMMON_DIR}/tools/net
  ${COMMON_DIR}/tools/net/utils
  ${COMMON_DIR}/tools/system
  ${COMMON_DIR}/tools/utils
)

set(COMMON
  ${COMMON_DIR}/tools/logger/Logger.cpp
  ${COMMON_DIR}/tools/net/SocketContext.cpp
  ${COMMON_DIR}/tools/net/SocketObject.cpp
  ${COMMON_DIR}/tools/net/Client.cpp
  ${COMMON_DIR}/tools/net/Connection.cpp
  ${COMMON_DIR}/tools/net/ConnectThread.cpp
  ${COMMON_DIR}/tools/net/Message.cpp
  ${COMMON_DIR}/tools/net/SimpleMessage.cpp
  ${COMMON_DIR}/tools/net/Server.cpp
  ${COMMON_DIR}/tools/net/utils/ConnectionChecker.cpp
  ${COMMON_DIR}/tools/system/Epool.cpp
  ${COMMON_DIR}/tools/system/PosixThread.cpp
  ${COMMON_DIR}/tools/thread/DelayedTask.cpp
  ${COMMON_DIR}/tools/thread/ThreadLoop.cpp
  ${COMMON_DIR}/tools/utils/Data.cpp
  ${COMMON_DIR}/tools/utils/DataResource.cpp
  ${COMMON_DIR}/tools/utils/TapeCutter.cpp
  ${SRC_DIR}/NetLogger.cpp
  ${SRC_DIR}/NetLoggerServer.cpp
  ${SRC_DIR}/NetLoggerClient.cpp
)

set(NETLOG
  ${COMMON}
  ${SRC_DIR}/NetLoggerLib.cpp
)

set(EXAMLE
  ${SRC_DIR}/example.cpp
)

set(NETREAD
  ${COMMON}
  ${SRC_DIR}/NetReader.cpp
)

add_library(netlog SHARED ${NETLOG})
target_link_libraries(netlog ${LD_FLAGS})

add_executable(example ${EXAMLE})
target_link_libraries(example ${LD_FLAGS})

add_executable(nread ${NETREAD})
target_link_libraries(nread ${LD_FLAGS})
