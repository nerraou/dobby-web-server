#pragma once

#include <istream>
#include <vector>

#include "ErrorPage.hpp"
#include "ConfigLocation.hpp"

class ConfigServer
{
private:
    int _port;
    std::vector<std::string> _serverNames;
    std::string _root;
    std::vector<ErrorPage> _errorPages;
    std::vector<std::string> _indexes;
    std::vector<ConfigLocation> _locationsContext;

public:
    ConfigServer();
    int getPort() const;
    void setPort(int port);
    const std::string &getServerName(size_t index);
    void addServerName(const std::string &serverName);
    const std::string &getRoot() const;
    void setRoot(const std::string &root);
    const std::string &getIndex(size_t index) const;
    void addIndex(const std::string &index);
    int getStatusCode() const;
    void setStatusCode(int status);
    const std::string &getErrorPagePath() const;
    void addErrorPagePath(const std::string &path);
    ~ConfigServer();
};