#pragma once

#include <vector>
#include <set>
#include <string>

#include "Server.hpp"
#include "ConfigServer.hpp"
#include "ServerSocket.hpp"
#include "HttpRequestHandler.hpp"

class ServerGroup
{
private:
    ServerSocket _socket;
    std::set<int> _connections;
    std::vector<Server *> _virtualServers;
    std::map<int, HttpRequestHandler *> _requests;

    bool hasConnection(int fd);
    int hasServerHost(const std::string &host) const;

public:
    ServerGroup();
    ServerGroup(int serverGroupPort);

    void addVirtualServer(const ConfigServer &configServer);
    int acceptConnection(void);
    void closeConnection(int &fd);
    int getServerIndex(const HttpParser &httpParser) const;
    void start(std::vector<PollFd> &connections);
    void handleTimeout(int nowTimestamp, int requestLastRead, int requestTimeout);

    ~ServerGroup();
};
