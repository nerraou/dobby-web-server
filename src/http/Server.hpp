#pragma once

#include <string>
#include <sstream>
#include <set>
#include <map>
#include <poll.h>

#include "HttpException.hpp"
#include "ServerSocket.hpp"
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

public:
    Server(const ConfigServer &config);
    const std::string &getRoot() const;
    bool isServerNameExist(const std::string &host) const;
    void start(HttpRequestHandler &HttpRequestHandler);
    bool resumeWriting(HttpRequestHandler &requestHandler);
    void setEnvVars(void);
    bool handleCGI(HttpRequestHandler &requestHandler, const std::string &path);
    ~Server();
};
