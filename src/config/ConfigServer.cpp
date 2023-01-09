#include "ConfigServer.hpp"

ConfigServer::ConfigServer() : _port(0)
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
const bool &ConfigServer::getAutoIndex() const
{
    return this->_autoIndex;
}

void ConfigServer::setAutoIndex(bool autoIndex)
{
    this->_autoIndex = autoIndex;
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

void ConfigServer::addLocationContext(ConfigLocation &location)
{
    this->_locationsContext.push_back(location);
}

bool ConfigServer::isGood() const
{
    if (this->_port == 0)
        return false;
    return true;
}

void ConfigServer::display(bool displayLocation) const
{
    std::cout << "Root " << this->getRoot() << std::endl;
    std::cout << "Port " << this->getPort() << std::endl;
    std::cout << "AutoIndex " << this->getAutoIndex() << std::endl;
    std::cout << "Indexes : \n";
    for (size_t i = 0; i < this->_indexes.size(); i++)
    {
        std::cout << this->_indexes[i] << std::endl;
    }
    std::cout << "Error Pages: \n";
    for (size_t i = 0; i < this->_errorPages.size(); i++)
    {
        size_t j = 0;
        while (j < this->_errorPages[i].statusCodes.size())
        {
            std::cout << this->_errorPages[i].statusCodes[j] << std::endl;
            j++;
        }

        std::cout << this->_errorPages[i].path << std::endl;
    }
    std::cout << "server Names: \n";
    for (size_t i = 0; i < this->_serverNames.size(); i++)
    {
        std::cout << this->_serverNames[i] << std::endl;
        
    }
    std::cout << "------------------------LOCATION CONTEXT \n";
    if (displayLocation)
   {
    for (size_t i = 0; i < this->_locationsContext.size(); i++)
    {
        this->_locationsContext[i].display();
    }
    
   }
    
}

ConfigServer::~ConfigServer()
{
}
