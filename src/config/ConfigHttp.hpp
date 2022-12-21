#pragma once

#include <iostream>
#include <vector>

#include "ErrorPage.hpp"
#include "ConfigServer.hpp"

class ConfigHttp
{
private:
    std::string _root;
    std::vector<std::string> _indexes;
    int _clientMaxBodySize;
    bool _autoIndex;
    std::vector<ConfigServer> _serversContext;
    std::vector<ErrorPage> _errorPages;

public:
    ConfigHttp();
    void addServerContext(const ConfigServer &server);
    const std::string &getRoot() const;
    void setRoot(const std::string &root);
    const std::string &getIndex(size_t index) const;
    void addIndex(const std::string &index);
    const int &getClientMaxBodySize() const;
    void setClientMaxBodySize(int size);
    const int &getStatusCode() const;
    void setStatusCode(int status);
    const std::string &getErrorPagePath() const;
    void addErrorPagePath(const std::string &path);
    bool getAutoIndex() const;
    void setAutoIndex(bool);
    ~ConfigHttp();
};
