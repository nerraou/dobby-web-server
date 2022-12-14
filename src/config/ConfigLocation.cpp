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

void ConfigLocation::addIndex(std::vector<std::string> index)
{
    this->_indexes = index;
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

void ConfigLocation::addAllowedHttpMethods(std::vector<std::string> method)
{
    this->_allowedHttpMethods = method;
}

const std::string &ConfigLocation::getRewrite() const
{
    return this->_rewrite;
}

void ConfigLocation::setRewrite(const std::string &rewrite)
{
    this->_rewrite = rewrite;
}

void ConfigLocation::setPath(const std::string &path)
{
    this->_path = path;
}

bool ConfigLocation::isGood() const
{
    if (this->_path.empty())
        return false;
    return true;
}

void ConfigLocation::display() const
{
    std::cout << "Root " << this->getRoot() << std::endl;
    std::cout << "AutoIndex " << this->getAutoIndex() << std::endl;
    std::cout << "Client Max body size " << this->getClientMaxBodySize() << std::endl;
    std::cout << "Indexes: \n";
    for (size_t i = 0; i < this->_indexes.size(); i++)
    {
        std::cout << " -"<< this->_indexes[i] << std::endl;
    }
    std::cout << "Accepted methods: \n";
    for (size_t i = 0; i < this->_allowedHttpMethods.size(); i++)
    {
        std::cout << "  -" << this->_allowedHttpMethods[i] << std::endl;
        
    }
    std::cout << "Rewrite " << this->_rewrite << std::endl;
    std::cout << "Path " << this->_path << std::endl;
}

ConfigLocation::~ConfigLocation()
{
}
