#include "ConfigHttp.hpp"

ConfigHttp::ConfigHttp()
{
}

const std::string &ConfigHttp::getRoot() const
{
    return this->_root;
}

void ConfigHttp::setRoot(const std::string &root)
{
    this->_root = root;
}

const std::string &ConfigHttp::getIndex(int indexOfIndex) const
{
    if (indexOfIndex < 0 || indexOfIndex >= this->_indexes.size())
        throw std::out_of_range("out of range");
    return this->_indexes[indexOfIndex];
}

void ConfigHttp::addIndex(const std::string &index)
{
    this->_indexes.push_back(index);
}

const int &ConfigHttp::getClientMaxBodySize() const
{
    return this->_clientMaxBodySize;
}

void ConfigHttp::setClientMaxBodySize(int size)
{
    this->_clientMaxBodySize = size;
}
// keep those methods until I found the correct way to be implement
const int &ConfigHttp::getStatusCode() const
{
}

void ConfigHttp::setStatusCode(int)
{
}

const std::string &ConfigHttp::getErrorPagePath() const
{
}

void ConfigHttp::addErrorPagePath(const std::string &path)
{
}

////////
const bool &ConfigHttp::getAutoIndex() const
{
    return this->_autoIndex;
}

void ConfigHttp::setAutoIndex(bool autoIndex)
{
    this->_autoIndex = autoIndex;
}

const int &ConfigHttp::getStatusCode() const
{
}

ConfigHttp::~ConfigHttp()
{
}
