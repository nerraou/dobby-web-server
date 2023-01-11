#pragma once

#include <string>
#include <sstream>
#include <set>
#include <map>
#include <poll.h>
#include "ServerSocket.hpp"
#include "Connection.hpp"
#include "ConfigServer.hpp"
#include "HttpRequestHandler.hpp"
#include "typedefs.hpp"

class Server
{
private:
    ServerSocket _socket;
    std::set<int> _connections;
    std::map<int, HttpRequestHandler> _requests;
    ConfigServer _config;

    void logAccess(const HttpRequestHandler &request, int httpStatus, std::size_t contentLength) const;

public:
    Server(const ConfigServer &config);
    ~Server();

    int acceptConnection(void);
    void closeConnection(int &fd);
    void start(std::vector<PollFd> &connections);
};
