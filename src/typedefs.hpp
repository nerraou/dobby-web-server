#pragma once

#include <poll.h>
#include <vector>
#include <map>
#include <string>

typedef struct pollfd PollFd;
typedef std::vector<unsigned char> ArrayBuffer;
typedef std::map<std::string, std::string> StringStringMap;
