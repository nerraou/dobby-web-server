#pragma once

#include <vector>
#include "ConfigHttp.hpp"
#include "Server.hpp"
#include "typedefs.hpp"

class Http
{
private:
    ConfigHttp _config;
    std::vector<Server *> _virtualServers;
    std::vector<PollFd> _connections;

public:
    Http(const ConfigHttp &config);
    void start(void);
    ~Http();
};
