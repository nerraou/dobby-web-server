#include "Config.hpp"

Config::Config()
{
}

Config &Config::operator=(ConfigServer const &other)
{
    this->_type = "serverContext";
    this->_port = other.getPort();
    this->_serverNames = other.getServerNames();
    this->_root = other.getRoot();
    this->_phpCGIPath = other.getPHPCGIPath();
    this->_indexes = other.getIndexes();
    this->_clientMaxBodySize = other.getClientMaxBodySize();
    this->_autoIndex = other.getAutoIndex();
    this->_errorPages = other.getErrorPages();
    return (*this);
}

Config &Config::operator=(ConfigLocation const &other)
{
    this->_type = "locationContext";
    this->_root = other.getRoot();
    this->_path = other.getPath();
    this->_phpCGIPath = other.getPHPCGIPath();
    this->_indexes = other.getIndexes();
    this->_clientMaxBodySize = other.getClientMaxBodySize();
    this->_autoIndex = other.getAutoIndex();
    this->_allowedHttpMethods = other.getAllowedHttpMethods();
    this->_rewrite = other.getRewrite();
    this->_errorPages = other.getErrorPages();
    return (*this);
}

const std::string &Config::getRewrite() const
{
    return this->_rewrite;
}

const std::string &Config::getRoot() const
{
    return this->_root;
}

const std::string &Config::getPhpCGIPath() const
{
    return this->_phpCGIPath;
}

const std::string &Config::getPath() const
{
    return this->_path;
}

const std::string &Config::getType() const
{
    return this->_type;
}

int Config::getPort() const
{
    return this->_port;
}

size_t Config::getClientMaxBodySize() const
{
    return this->_clientMaxBodySize;
}

bool Config::getAutoIndex() const
{
    return this->_autoIndex;
}

const std::vector<std::string> &Config::getIndexes() const
{
    return this->_indexes;
}

const std::vector<std::string> &Config::getServerNames() const
{
    return this->_serverNames;
}

Config::~Config()
{
}
