#pragma once

#include <vector>
#include <bits/stdc++.h>
#include <algorithm>
#include "ConfigHttp.hpp"
#include "Server.hpp"
#include "HttpRequestHandler.hpp"
#include "typedefs.hpp"

class Http
{
private:
    ConfigHttp _config;
    std::vector<Server *> _virtualServers;
    std::vector<PollFd> _connections;

    inline static bool comparePollFdByFd(const PollFd &a, const PollFd &b);
    inline static bool isBadPollFd(const PollFd &a);
    inline void removeBadConnections(void);

public:
    Http(const ConfigHttp &config);
    void start(void);
    ~Http();
};
