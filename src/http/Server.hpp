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
    std ::string getErrorPagePath(int status);
    bool isServerNameExist(const std::string &host) const;
    void start(HttpRequestHandler &HttpRequestHandler);
    bool resumeWriting(HttpRequestHandler &requestHandler);
    void setEnvVars(void);
    bool handleCGI(HttpRequestHandler &requestHandler, const std::string &path);
    void executeMethods(HttpRequestHandler &requestHandler, const std::string &path);
    void initConfig();
    ~Server();
};
