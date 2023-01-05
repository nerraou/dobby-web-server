#pragma once

#include <istream>
#include <vector>

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
    void addIndex(const std::string &index);
    const size_t &getClientMaxBodySize() const;
    void setClientMaxBodySize(size_t size);
    const bool &getAutoIndex() const;
    void setAutoIndex(bool);
    const std::string &getAllowedHttpMethods(int index);
    void addAllowedHttpMethods(const std::string &method);
    const std::string &getRewrite() const;
    void setRewrite(const std::string &rewrite);
    bool isGood() const;
    ~ConfigLocation();
};
