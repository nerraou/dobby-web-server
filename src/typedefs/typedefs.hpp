#pragma once

#include <poll.h>
#include <vector>
#include <map>
#include <string>

#define KB 1024
#define MB 1048576

#define DEFAULT_CONFIG_PATH "./assets/default-config/default-config"

typedef struct pollfd PollFd;
typedef std::vector<unsigned char> ArrayBuffer;
typedef std::map<std::string, std::string> StringStringMap;
