#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation()
{
}

const std::string &ConfigLocation::getRoot() const
{
    return this->_root;
}

void ConfigLocation::setRoot(const std::string &root)
{
    this->_root = root;
}

const std::string &ConfigLocation::getIndex(size_t index) const
{
    if (index >= this->_indexes.size())
        throw std::out_of_range("out of range");
    return this->_indexes[index];
}

void ConfigLocation::addIndex(const std::string &index)
{
    this->_indexes.push_back(index);
}

const size_t &ConfigLocation::getClientMaxBodySize() const
{
    return this->_clientMaxBodySize;
}

void ConfigLocation::setClientMaxBodySize(size_t size)
{
    this->_clientMaxBodySize = size;
}

const bool &ConfigLocation::getAutoIndex() const
{
    return this->_autoIndex;
}

void ConfigLocation::setAutoIndex(bool autoIndex)
{
    this->_autoIndex = autoIndex;
}

const std::string &ConfigLocation::getAllowedHttpMethods(int index)
{
    if (index < 0 || (size_t)index >= this->_indexes.size())
        throw std::out_of_range("out of range");
    return this->_allowedHttpMethods[index];
}

void ConfigLocation::addAllowedHttpMethods(const std::string &method)
{
    this->_allowedHttpMethods.push_back(method);
}

const std::string &ConfigLocation::getRewrite() const
{
    return this->_rewrite;
}

void ConfigLocation::setRewrite(const std::string &rewrite)
{
    this->_rewrite = rewrite;
}

bool ConfigLocation::isGood()
{
    if (this->_path.size() == 0)
        return false;
    return true;
}

ConfigLocation::~ConfigLocation()
{
}
