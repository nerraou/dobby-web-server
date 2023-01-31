#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"

class Config
{
private:
    int _port;
    std::vector<std::string> _serverNames;
    std::map<int, std::string> _errorPages;
    std::string _type;
    std::string _root;
    std::string _phpCGIPath;
    std::vector<std::string> _indexes;
    size_t _clientMaxBodySize;
    bool _autoIndex;
    std::vector<std::string> _allowedHttpMethods;
    std::string _rewrite;
    std::string _path;

public:
    Config();
    Config &operator=(ConfigServer const &other);
    Config &operator=(ConfigLocation const &other);

    const std::string &getType() const;
    const std::string &getPath() const;
    const std::string &getRewrite() const;
    const std::string &getRoot() const;
    const std::string &getPhpCGIPath() const;
    int getPort() const;
    size_t getClientMaxBodySize() const;
    bool getAutoIndex() const;
    const std::vector<std::string> &getIndexes() const;
    const std::vector<std::string> &getServerNames() const;

    ~Config();
};
