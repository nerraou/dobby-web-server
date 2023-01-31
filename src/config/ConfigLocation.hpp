#pragma once

#include <istream>
#include <vector>
#include <iostream>
#include <map>
#include "ConfigServer.hpp"

class ConfigServer;

class ConfigLocation
{
private:
    std::string _root;
    std::string _path;
    std::string _phpCGIPath;
    std::vector<std::string> _indexes;
    size_t _clientMaxBodySize;
    bool _autoIndex;
    std::vector<std::string> _allowedHttpMethods;
    std::map<int, std::string> _errorPages;
    std::string _rewrite;

public:
    ConfigLocation();

    bool operator<(ConfigLocation const &other) const;
    const std::string &getRoot() const;
    void setRoot(const std::string &root);
    const std::string &getPHPCGIPath(void) const;
    void setPHPCGIPath(const std::string &path);
    const std::string &getIndex(size_t index) const;
    void addIndex(std::vector<std::string> index);
    const std::vector<std::string> &getIndexes() const;
    const size_t &getClientMaxBodySize() const;
    void setErrorPagesFromList(const std::vector<std::string> &statuses, const std::string &path);
    void setErrorPages(const std::map<int, std::string> &errorPages);
    void addErrorPage(int status, const std::string &path);
    const std::map<int, std::string> &getErrorPages() const;
    void setClientMaxBodySize(size_t size);
    const bool &getAutoIndex() const;
    void setAutoIndex(bool);
    const std::string &getAllowedHttpMethod(int index) const;
    const std::vector<std::string> &getAllowedHttpMethods() const;
    void addAllowedHttpMethods(std::vector<std::string> method);
    const std::string &getRewrite() const;
    void setRewrite(const std::string &rewrite);
    void setPath(const std::string &path);
    std::string getPath() const;
    bool isGood() const;
    void inherit(const ConfigServer &configServer);
    void display() const;
    ~ConfigLocation();
};
