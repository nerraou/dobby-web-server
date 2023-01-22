#pragma once

#include <vector>
#include <bits/stdc++.h>
#include <algorithm>
#include "ConfigHttp.hpp"
#include "Server.hpp"
#include "HttpRequestHandler.hpp"
#include "typedefs.hpp"
#include "ServerGroup.hpp"

class Http
{
private:
    ConfigHttp _config;
    std::vector<PollFd> _connections;
    std::map<int, ServerGroup *> _serverGroups;

    inline static bool comparePollFdByFd(const PollFd &a, const PollFd &b);
    inline static bool isBadPollFd(const PollFd &a);
    inline void removeBadConnections(void);
    void createServerGroups();

public:
    Http(const ConfigHttp &config);
    void start(void);
    ~Http();
};
