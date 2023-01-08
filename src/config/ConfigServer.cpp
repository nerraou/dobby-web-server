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

const std::string &ConfigServer::getServerName(size_t index)
{
    if (index >= this->_serverNames.size())
        throw std::out_of_range("out of range");
    return this->_serverNames[index];
}

void ConfigServer::setErrorPage(std::vector<std::string> errorPage)
{
    ErrorPage preparedErrorPage;
    size_t i;

    for (i = 0; i < errorPage.size() - 1; i++)
    {
        preparedErrorPage.statusCodes.push_back(std::atoi(errorPage[i].c_str()));
    }
    preparedErrorPage.path = errorPage[i];
    this->_errorPages.push_back(preparedErrorPage);
}

void ConfigServer::addServerNames(std::vector<std::string> serverNames)
{
    this->_serverNames = serverNames;
}

const std::string &ConfigServer::getRoot() const
{
    return this->_root;
}

void ConfigServer::setRoot(const std::string &root)
{
    this->_root = root;
}

const std::string &ConfigServer::getIndex(size_t indexOfIndex) const
{
    if (indexOfIndex >= this->_indexes.size())
        throw std::out_of_range("out of range");
    return this->_indexes[indexOfIndex];
}

void ConfigServer::addIndex(std::vector<std::string> index)
{
    this->_indexes = index;
}

bool ConfigServer::isGood() const
{
    if (this->_port == 0)
        return false;
    return true;
}

ConfigServer::~ConfigServer()
{
}
