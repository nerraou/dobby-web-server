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

std::size_t ConfigServer::getServerNamesCount(void) const
{
    return this->_serverNames.size();
}

const std::vector<std::string> ConfigServer::getServerNames() const
{
    return this->_serverNames;
}

const std::string &ConfigServer::getServerName(size_t index)
{
    return this->_serverNames.at(index);
}

bool ConfigServer::isServerNameExist(const std::string &host) const
{
    return std::find(this->_serverNames.begin(), this->_serverNames.end(), host) != this->_serverNames.end();
}

void ConfigServer::setErrorPages(const std::map<int, std::string> &errorPages)
{
    this->_errorPages = errorPages;
}

void ConfigServer::setErrorPagesFromList(const std::vector<std::string> &statuses, const std::string &path)
{
    int status;

    for (size_t i = 0; i < statuses.size(); i++)
    {
        status = ::atoi(statuses.at(i).c_str());
        this->_errorPages[status] = path;
    }
}

const std::map<int, std::string> &ConfigServer::getErrorPages() const
{
    return this->_errorPages;
}

void ConfigServer::addErrorPage(int status, const std::string &path)
{
    if (this->_errorPages.count(status) == 0)
        this->_errorPages[status] = path;
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

const std::string &ConfigServer::getPHPCGIPath(void) const
{
    return this->_phpCGIPath;
}

void ConfigServer::setPHPCGIPath(const std::string &path)
{
    this->_phpCGIPath = path;
}

const std::vector<std::string> &ConfigServer::getIndexes() const
{
    return this->_indexes;
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

const size_t &ConfigServer::getClientMaxBodySize() const
{
    return this->_clientMaxBodySize;
}

void ConfigServer::setClientMaxBodySize(size_t size)
{
    this->_clientMaxBodySize = size;
}

void ConfigServer::addLocationContext(ConfigLocation &location)
{
    this->_locationsContext.push_back(location);
}

int ConfigServer::findLocationPathMatch(const std::string &path) const
{
    for (size_t i = 0; i < this->_locationsContext.size(); i++)
    {
        if (lib::startsWith(path, this->_locationsContext.at(i).getPath()))
            return i;
    }
    return -1;
}

const ConfigLocation &ConfigServer::getConfigLocation(int index) const
{
    return (this->_locationsContext.at(index));
}

bool ConfigServer::isGood() const
{
    if (this->_port == 0)
        return false;
    return true;
}

void ConfigServer::inherit(const ConfigHttp &configHttp)
{
    this->setRoot(configHttp.getRoot());
    this->setAutoIndex(configHttp.getAutoIndex());
    this->setClientMaxBodySize(configHttp.getClientMaxBodySize());
    this->_indexes = configHttp.getIndexes();
    this->_errorPages = configHttp.getErrorPages();
    this->_phpCGIPath = configHttp.getPHPCGIPath();
}

void ConfigServer::display(bool displayLocation) const
{
    std::cout << "Root " << this->getRoot() << std::endl;
    std::cout << "Port " << this->getPort() << std::endl;
    std::cout << "AutoIndex " << this->getAutoIndex() << std::endl;
    std::cout << "Client Max body size " << this->getClientMaxBodySize() << std::endl;

    std::cout << "Indexes: \n";
    for (size_t i = 0; i < this->_indexes.size(); i++)
    {
        std::cout << " -" << this->_indexes[i] << std::endl;
    }

    std::cout << "Error Pages:\n";
    for (std::map<int, std::string>::const_iterator i = this->_errorPages.begin(); i != this->_errorPages.end(); ++i)
    {
        std::cout << " -" << i->first << ": " << i->second << std::endl;
    }

    std::cout << "server Names: \n";
    for (size_t i = 0; i < this->_serverNames.size(); i++)
    {
        std::cout << " -" << this->_serverNames[i] << std::endl;
    }
    std::cout << "------------------------LOCATION CONTEXT \n";
    if (displayLocation)
    {
        for (size_t i = 0; i < this->_locationsContext.size(); i++)
        {
            this->_locationsContext[i].display();
            std::cout << std::endl;
        }
    }
}

void ConfigServer::sortLocationByPath()
{
    std::sort(this->_locationsContext.begin(), this->_locationsContext.end());
}

ConfigServer::~ConfigServer()
{
}
