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

void ConfigHttp::addIndex(std::vector<std::string> index)
{
    this->_indexes = index;
}

const size_t &ConfigHttp::getClientMaxBodySize() const
{
    return this->_clientMaxBodySize;
}

void ConfigHttp::setClientMaxBodySize(size_t size)
{
    this->_clientMaxBodySize = size;
}

void ConfigHttp::setErrorPage(std::vector<std::string> errorPage)
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

bool ConfigHttp::isGood() const
{
    if (this->_root.empty() || this->_serversContext.size() == 0)
        return false;
    return true;
}

void ConfigHttp::display(bool displayServer) const
{
    std::cout << "Root " << this->getRoot() << std::endl;
    std::cout << "Client Max body size " << this->getClientMaxBodySize() << std::endl;
    std::cout << "AutoIndex " << this->getAutoIndex() << std::endl;
    std::cout << "Indexes "; 
    for (size_t i = 0; i < this->_indexes.size(); i++)
    {
        std::cout << this->_indexes[i] << std::endl;
    }
    for (size_t i = 0; i < this->_errorPages.size(); i++)
    {
        size_t j= 0;
        while (j < this->_errorPages[i].statusCodes.size())
        {
        std::cout << this->_errorPages[i].statusCodes[j] << std::endl;
            j++;
        }
        
        std::cout << this->_errorPages[i].path << std::endl;
    }
    if (displayServer)
    {
     
    std::cout << "------------------------SERVER  CONTEXT \n";
        for (size_t i = 0; i < this->_serversContext.size(); i++)
        {
            this->_serversContext[i].display(true);
        }
        
    }
}

ConfigHttp::~ConfigHttp()
{
}
