#pragma once

#include <string>
#include <sstream>
#include <set>
#include <poll.h>
#include "ServerSocket.hpp"
#include "Connection.hpp"
#include "ConfigServer.hpp"
#include "typedefs.hpp"

class Server
{
private:
    ServerSocket _socket;
    std::set<int> _connections;
    ConfigServer _config;

public:
    Server(const ConfigServer &config);
    ~Server();

    int acceptConnection(void);
    void start(std::vector<PollFd> &connections);
};
