#pragma once

#include <istream>
#include <vector>
#include <algorithm>
#include <map>
#include <algorithm>
#include <cstdlib>

#include "ConfigLocation.hpp"
#include "ConfigHttp.hpp"
#include "lib.hpp"

class ConfigHttp;
class ConfigLocation;

class ConfigServer
{
private:
    int _port;
    std::vector<std::string> _serverNames;
    std::string _root;
    bool _autoIndex;
    std::map<int, std::string> _errorPages;
    std::map<std::string, std::string> _cgi;
    std::vector<std::string> _indexes;
    size_t _clientMaxBodySize;
    std::vector<ConfigLocation> _locationsContext;

public:
    ConfigServer();
    void init();
    int getPort() const;
    void setPort(int port);

    const bool &getAutoIndex() const;
    void setAutoIndex(bool);

    const std::string &getServerName(size_t index);
    std::size_t getServerNamesCount(void) const;

    const std::vector<std::string> getServerNames() const;
    bool isServerNameExist(const std::string &host) const;
    void addServerNames(std::vector<std::string> serverNames);

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

    void addLocationContext(ConfigLocation &location);
    int findLocationPathMatch(const std::string &path) const;
    const ConfigLocation &getConfigLocation(int index) const;

    bool isGood() const;
    void inherit(const ConfigHttp &configHttp);
    void display(bool displayLocation) const;
    void sortLocationByPath();

    ~ConfigServer();
};
