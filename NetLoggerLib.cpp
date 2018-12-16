#include "NetLogger.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>

std::shared_ptr<NetLogger> g_logger;
std::shared_ptr<Connection> g_connection;

extern "C" bool init(int port, size_t max_queue) {
  if(!g_logger) {
    g_logger = std::make_shared<NetLogger>(max_queue);
    g_connection = std::make_shared<Connection>();
    g_connection->Init();
    return g_logger->Init(port, g_connection);
  }
  return false;
}

extern "C" void log_msg(const char* msg) {
  if(g_logger)
    g_logger->Log(msg);
}
