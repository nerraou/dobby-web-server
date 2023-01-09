#pragma once

#include <istream>
#include <vector>
#include <iostream>

class ConfigLocation
{
private:
    std::string _root;
    std::vector<std::string> _indexes;
    size_t _clientMaxBodySize;
    bool _autoIndex;
    std::vector<std::string> _allowedHttpMethods;
    std::string _rewrite;
    std::string _path;

public:
    ConfigLocation();
    const std::string &getRoot() const;
    void setRoot(const std::string &root);
    const std::string &getIndex(size_t index) const;
    void addIndex(std::vector<std::string> index);
    const size_t &getClientMaxBodySize() const;
    void setClientMaxBodySize(size_t size);
    const bool &getAutoIndex() const;
    void setAutoIndex(bool);
    const std::string &getAllowedHttpMethods(int index);
    void addAllowedHttpMethods(std::vector<std::string> method);
    const std::string &getRewrite() const;
    void setRewrite(const std::string &rewrite);
    void setPath(const std::string &path);
    std::string getPath() const;
    bool isGood() const;
    void display() const;
    ~ConfigLocation();
};
