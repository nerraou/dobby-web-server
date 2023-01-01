#include "ConfigHttp.hpp"

ConfigHttp::ConfigHttp()
{
}

void ConfigHttp::addServerContext(ConfigServer &server)
{
    this->_serversContext.push_back(server);
}

const std::string &ConfigHttp::getRoot() const
{
    return this->_root;
}

void ConfigHttp::setRoot(const std::string &root)
{
    this->_root = root;
}

const std::string &ConfigHttp::getIndex(size_t indexOfIndex) const
{
    if (indexOfIndex >= this->_indexes.size())
        throw std::out_of_range("out of range");
    return this->_indexes[indexOfIndex];
}

void ConfigHttp::addIndex(const std::string &index)
{
    this->_indexes.push_back(index);
}

const size_t &ConfigHttp::getClientMaxBodySize() const
{
    return this->_clientMaxBodySize;
}

void ConfigHttp::setClientMaxBodySize(size_t size)
{
    this->_clientMaxBodySize = size;
}

bool ConfigHttp::getAutoIndex() const
{
    return this->_autoIndex;
}

void ConfigHttp::setAutoIndex(bool autoIndex)
{
    this->_autoIndex = autoIndex;
}

size_t ConfigHttp::getServersCount(void) const
{
    return this->_serversContext.size();
}

ConfigServer &ConfigHttp::getServerConfig(size_t index)
{
    return this->_serversContext[index];
}

ConfigHttp::~ConfigHttp()
{
}
