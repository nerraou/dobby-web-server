#pragma once

#include <istream>
#include <vector>
#include <cstdlib>

#include "ErrorPage.hpp"
#include "ConfigLocation.hpp"

class ConfigServer
{
private:
    int _port;
    std::vector<std::string> _serverNames;
    std::string _root;
    bool _autoIndex;
    std::vector<ErrorPage> _errorPages;
    std::vector<std::string> _indexes;
    std::vector<ConfigLocation> _locationsContext;

public:
    ConfigServer();
    int getPort() const;
    void setPort(int port);
    const bool &getAutoIndex() const;
    void setAutoIndex(bool);
    const std::string &getServerName(size_t index);
    void addServerNames(std::vector<std::string> serverNames);
    const std::string &getRoot() const;
    void setRoot(const std::string &root);
    const std::string &getIndex(size_t index) const;
    void addIndex(std::vector<std::string> index);
    int getStatusCode() const;
    void setStatusCode(int status);
    void setErrorPage(std::vector<std::string> errorPage);
    const std::string &getErrorPagePath() const;
    void addErrorPagePath(const std::string &path);
    void addLocationContext(ConfigLocation &location);
    bool isGood() const;
    ~ConfigServer();
};
