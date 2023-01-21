#pragma once

#include <vector>
#include <set>
#include "Server.hpp"
#include "configServer.hpp"
#include "HttpRequestHandler.hpp"

class ServerGroup
{
private:
    ServerSocket _socket;
    ConfigServer _config;
    std::set<int> _connections;
    std::vector<Server *> _virtualServers;
    std::map<int, HttpRequestHandler> _requests;

    bool hasConnection(int fd);
public:
    ServerGroup();
    ServerGroup(const ConfigServer &configServer);
    void addServerGroup(const ConfigServer &configServer);
    int acceptConnection(void);
    void closeConnection(int &fd);
    void start(std::vector<PollFd> &connections);
    void handleTimeout(int nowTimestamp, int requestLastRead, int requestTimeout);
    ~ServerGroup();
};
