#include "ConfigHttp.hpp"

ConfigHttp::ConfigHttp()
{
    this->_clientMaxBodySize = MB;
    this->_autoIndex = false;
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

const std::vector<std::string> &ConfigHttp::getIndexes() const
{
    return this->_indexes;
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

void ConfigHttp::setErrorPagesFromList(const std::vector<std::string> &statuses, const std::string &path)
{
    int status;

    for (size_t i = 0; i < statuses.size(); i++)
    {
        status = ::atoi(statuses.at(i).c_str());
        this->_errorPages[status] = path;
    }
}

void ConfigHttp::addErrorPage(int status, const std::string &path)
{
    if (this->_errorPages.count(status) == 0)
        this->_errorPages[status] = path;
}

void ConfigHttp::addCGI(const std::vector<std::string> &cgi)
{
    this->_cgi.insert(std::pair<std::string, std::string>(cgi.at(1), cgi.at(0)));
}

const std::map<std::string, std::string> &ConfigHttp::getCGI() const
{
    return this->_cgi;
}

void ConfigHttp::setErrorPages(const std::map<int, std::string> &errorPages)
{
    this->_errorPages = errorPages;
}

const std::map<int, std::string> &ConfigHttp::getErrorPages() const
{
    return this->_errorPages;
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
    std::cout << "CGI :\n";
    for (std::map<std::string, std::string>::const_iterator i = this->_cgi.begin(); i != this->_cgi.end(); ++i)
    {
        std::cout << " -PathCGI: " << i->first << "  -CGIextension: " << i->second << std::endl;
    }

    if (displayServer)
    {

        std::cout << "------------------------SERVER  CONTEXT \n";
        for (size_t i = 0; i < this->_serversContext.size(); i++)
        {
            this->_serversContext[i].display(true);
            std::cout << std::endl;
        }
    }
}

ConfigHttp::~ConfigHttp()
{
}
