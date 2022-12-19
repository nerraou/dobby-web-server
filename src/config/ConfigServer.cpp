#include "ConfigServer.hpp"

ConfigServer::ConfigServer()
{
}

int ConfigServer::getPort() const
{
    return this->_port;
}

void ConfigServer::setPort(int port)
{
    this->_port = port;
}

const std::string &ConfigServer::getServerName(int index)
{
    if (index < 0 || index >= this->_serverNames.size())
        throw std::out_of_range("out of range");
    return this->_serverNames[index];
}

void ConfigServer::addServerName(const std::string &serverName)
{
    this->_serverNames.push_back(serverName);
}

const std::string &ConfigServer::getRoot() const
{
    return this->_root;
}

void ConfigServer::setRoot(const std::string &root)
{
    this->_root = root;
}

const std::string &ConfigServer::getIndex(int indexOfIndex) const
{
    if (indexOfIndex < 0 || indexOfIndex >= this->_indexes.size())
        throw std::out_of_range("out of range");
    return this->_indexes[indexOfIndex];
}

void ConfigServer::addIndex(const std::string &index)
{
    this->_indexes.push_back(index);
}

// keep those methods until I found the correct way to be implement
int ConfigServer::getStatusCode() const
{
}

void ConfigServer::setStatusCode(int)
{
}

const std::string &ConfigServer::getErrorPagePath() const
{
}

void ConfigServer::addErrorPagePath(const std::string &path)
{
}

////////
ConfigServer::~ConfigServer()
{
}
