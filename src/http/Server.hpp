#pragma once

#include <string>
#include <sstream>
#include <set>
#include <map>
#include <poll.h>

#include "HttpException.hpp"
#include "ConfigServer.hpp"
#include "HttpRequestHandler.hpp"
#include "typedefs.hpp"
#include "Config.hpp"

class Server
{
private:
    ConfigServer _configServer;
    Config _config;
    int findLocationPathMatch(const std::string &path) const;
    const ConfigLocation &getConfigLocation(int index) const;

public:
    Server(const ConfigServer &config);
    const std::string &getRoot() const;
    std ::string getErrorPagePath(int status) const;
    bool isServerNameExist(const std::string &host) const;
    void start(HttpRequestHandler &HttpRequestHandler);
    bool resumeWriting(HttpRequestHandler &requestHandler, std::size_t clientMaxBodySize);
    void setEnvVars(void);
    void handleClientMaxBodySize(const HttpRequestHandler &requestHandler);
    void executeMethods(HttpRequestHandler &requestHandler, const std::string &path);
    std::string resolvePath(const std::string &matchedLocationPath, const std::string &requestPath);
    void initConfig();
    const Config &getConfig(void) const;
    ~Server();
};
