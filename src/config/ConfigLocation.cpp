#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation()
{
}

bool ConfigLocation::operator<(ConfigLocation const &other) const
{
    return other.getPath().compare(this->getPath()) < 0;
}

void ConfigLocation::init()
{
   this->_root.clear();
   this->_path.clear();
   this->_phpCGIPath.clear();
   this->_rewrite.url.clear();
   this->_rewrite.status = -1;
   this->_clientMaxBodySize = 0;
   this->_autoIndex = true;
   this->_indexes.clear();
   this->_allowedHttpMethods.clear();
   this->_errorPages.clear();
}

const std::string &ConfigLocation::getRoot() const
{
    return this->_root;
}

void ConfigLocation::setRoot(const std::string &root)
{
    this->_root = root;
}

const std::string &ConfigLocation::getPHPCGIPath(void) const
{
    return this->_phpCGIPath;
}

void ConfigLocation::setPHPCGIPath(const std::string &path)
{
    this->_phpCGIPath = path;
}

const std::string &ConfigLocation::getIndex(size_t index) const
{
    if (index >= this->_indexes.size())
        throw std::out_of_range("out of range");
    return this->_indexes[index];
}

void ConfigLocation::addIndex(std::vector<std::string> index)
{
    this->_indexes = index;
}

const std::vector<std::string> &ConfigLocation::getIndexes() const
{
    return this->_indexes;
}

const size_t &ConfigLocation::getClientMaxBodySize() const
{
    return this->_clientMaxBodySize;
}

void ConfigLocation::setErrorPagesFromList(const std::vector<std::string> &statuses, const std::string &path)
{
    int status;

    for (size_t i = 0; i < statuses.size(); i++)
    {
        status = ::atoi(statuses.at(i).c_str());
        this->_errorPages[status] = path;
    }
}

void ConfigLocation::setErrorPages(const std::map<int, std::string> &errorPages)
{
    this->_errorPages = errorPages;
}

void ConfigLocation::addErrorPage(int status, const std::string &path)
{
    if (this->_errorPages.count(status) == 0)
        this->_errorPages[status] = path;
}

const std::map<int, std::string> &ConfigLocation::getErrorPages() const
{
    return this->_errorPages;
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

const std::string &ConfigLocation::getAllowedHttpMethod(int index) const
{
    if (index < 0 || (size_t)index >= this->_indexes.size())
        throw std::out_of_range("out of range");
    return this->_allowedHttpMethods[index];
}

const std::vector<std::string> &ConfigLocation::getAllowedHttpMethods() const
{
    return this->_allowedHttpMethods;
}

void ConfigLocation::addAllowedHttpMethods(std::vector<std::string> method)
{
    this->_allowedHttpMethods = method;
}

const Rewrite &ConfigLocation::getRewrite() const
{
    return this->_rewrite;
}

void ConfigLocation::setRewrite(const std::vector<std::string> &rewrite)
{
    this->_rewrite.url = rewrite.at(0);
    if (rewrite.at(1) == "redirect")
        this->_rewrite.status = 307;
    else if (rewrite.at(1) == "permanent")
        this->_rewrite.status = 308;
    else
        throw ConfigException("Rewrite Flag invalid, try redirect/permanent.");
}

void ConfigLocation::setPath(const std::string &path)
{
    this->_path = path;
}

std::string ConfigLocation::getPath() const
{
    return this->_path;
}

bool ConfigLocation::isGood() const
{
    if (this->_path.empty())
        return false;
    return true;
}

void ConfigLocation::inherit(const ConfigServer &configServer)
{
    this->setRoot(configServer.getRoot());
    this->setAutoIndex(configServer.getAutoIndex());
    this->setClientMaxBodySize(configServer.getClientMaxBodySize());
    this->_indexes = configServer.getIndexes();
    this->_phpCGIPath = configServer.getPHPCGIPath();
}

void ConfigLocation::display() const
{
    std::cout << "Root " << this->getRoot() << std::endl;
    std::cout << "AutoIndex " << this->getAutoIndex() << std::endl;
    std::cout << "Client Max body size " << this->getClientMaxBodySize() << std::endl;
    std::cout << "Indexes: \n";
    for (size_t i = 0; i < this->_indexes.size(); i++)
    {
        std::cout << " -" << this->_indexes[i] << std::endl;
    }
    std::cout << "Accepted methods: \n";
    for (size_t i = 0; i < this->_allowedHttpMethods.size(); i++)
    {
        std::cout << "  -" << this->_allowedHttpMethods[i] << std::endl;
    }
    std::cout << "Rewrite status " << this->_rewrite.status << std::endl;
    std::cout << "Rewrite url " << this->_rewrite.url << std::endl;
    std::cout << "Path " << this->_path << std::endl;
}

ConfigLocation::~ConfigLocation()
{
}
