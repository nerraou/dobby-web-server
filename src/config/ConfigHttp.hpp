#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#include "ConfigServer.hpp"
#include "typedefs.hpp"
class ConfigServer;

class ConfigHttp
{
private:
    std::string _root;
    std::vector<std::string> _indexes;
    size_t _clientMaxBodySize;
    bool _autoIndex;
    std::vector<ConfigServer> _serversContext;
    std::map<int, std::string> _errorPages;
    std::map<std::string, std::string> _cgi;

public:
    ConfigHttp();
    void addServerContext(ConfigServer &server);

    const std::string &getRoot() const;
    void setRoot(const std::string &root);

    const std::vector<std::string> &getIndexes() const;
    const std::string &getIndex(size_t index) const;
    void addIndex(std::vector<std::string> index);

    const size_t &getClientMaxBodySize() const;
    void setClientMaxBodySize(size_t size);

    void setErrorPagesFromList(const std::vector<std::string> &statuses, const std::string &path);
    void setErrorPages(const std::map<int, std::string> &errorPages);
    void addErrorPage(int status, const std::string &path);
    const std::map<int, std::string> &getErrorPages() const;

    void addCGI(const std::vector<std::string> &cgi);
    const std::map<std::string, std::string> &getCGI() const;

    bool getAutoIndex() const;
    void setAutoIndex(bool);

    size_t getServersCount(void) const;

    ConfigServer &getServerConfig(size_t index);
    bool isGood() const;
    void display(bool displayServer) const;

    ~ConfigHttp();
};
