#pragma once

#include <istream>
#include <vector>

class ConfigLocation
{
private:
    std::string _root;
    std::vector<std::string> _indexes;
    int _clientMaxBodySize;
    bool _autoIndex;
    std::vector<std::string> _allowedHttpMethods;
    std::string _rewrite;

public:
    ConfigLocation();
    const std::string &getRoot() const;
    void setRoot(const std::string &root);
    const std::string &getIndex(size_t index) const;
    void addIndex(const std::string &index);
    const int &getClientMaxBodySize() const;
    void setClientMaxBodySize(int size);
    const bool &getAutoIndex() const;
    void setAutoIndex(bool);
    const std::string &getAllowedHttpMethods(int index);
    void addAllowedHttpMethods(const std::string &method);
    const std::string &getRewrite() const;
    void setRewrite(const std::string &rewrite);
    ~ConfigLocation();
};
