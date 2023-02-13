#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"
#include "httpStatus.hpp"

#define ROOT_DEFAULT_ERROR_PAGES "./assets/deafult-error-pages/"

class Config
{
private:
    int _port;
    std::vector<std::string> _serverNames;
    std::map<int, std::string> _errorPages;
    std::map<int, std::string> _defaultErrorPages;
    std::string _type;
    std::string _root;
    std::vector<std::string> _indexes;
    size_t _clientMaxBodySize;
    bool _autoIndex;
    std::vector<std::string> _allowedHttpMethods;
    Rewrite _rewrite;
    std::string _path;
    std::map<std::string, std::string> _cgi;

private:
    bool hasErrorPage(int status) const;
    void initDefaultErrorPages();

public:
    Config();
    Config &operator=(ConfigServer const &other);
    Config &operator=(ConfigLocation const &other);

    std::string getErrorPagePath(int status) const;

    const std::string &getType() const;

    const std::string &getPath() const;

    const Rewrite &getRewrite() const;

    const std::string &getRoot() const;

    int getPort() const;

    size_t getClientMaxBodySize() const;

    bool getAutoIndex() const;

    bool hasRewrite() const;

    const std::vector<std::string> &getIndexes() const;

    const std::vector<std::string> &getServerNames() const;

    bool hasMethod(const std::string &method);

    bool hasCGI(const std::string &path) const;

    const std::string &getCGIPath(const std::string &path) const;

    ~Config();
};
