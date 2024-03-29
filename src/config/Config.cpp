#include "Config.hpp"

Config::Config()
{
    this->initDefaultErrorPages();
}

Config &Config::operator=(ConfigServer const &other)
{
    this->_type = "serverContext";
    this->_port = other.getPort();
    this->_serverNames = other.getServerNames();
    this->_root = other.getRoot();
    this->_indexes = other.getIndexes();
    this->_clientMaxBodySize = other.getClientMaxBodySize();
    this->_autoIndex = other.getAutoIndex();
    this->_errorPages = other.getErrorPages();
    this->_cgi = other.getCGI();
    return (*this);
}

Config &Config::operator=(ConfigLocation const &other)
{
    this->_type = "locationContext";
    this->_root = other.getRoot();
    this->_path = other.getPath();
    this->_indexes = other.getIndexes();
    this->_clientMaxBodySize = other.getClientMaxBodySize();
    this->_autoIndex = other.getAutoIndex();
    this->_allowedHttpMethods = other.getAllowedHttpMethods();
    this->_rewrite = other.getRewrite();
    this->_errorPages = other.getErrorPages();
    this->_cgi = other.getCGI();

    return (*this);
}

void Config::initDefaultErrorPages()
{
    this->_defaultErrorPages.insert(std::pair<int, std::string>(HTTP_BAD_REQUEST, "default-400.html"));
    this->_defaultErrorPages.insert(std::pair<int, std::string>(HTTP_FORBIDDEN, "default-403.html"));
    this->_defaultErrorPages.insert(std::pair<int, std::string>(HTTP_NOT_FOUND, "default-404.html"));
    this->_defaultErrorPages.insert(std::pair<int, std::string>(HTTP_METHOD_NOT_ALLOWED, "default-405.html"));
    this->_defaultErrorPages.insert(std::pair<int, std::string>(HTTP_REQUEST_TIMEOUT, "default-408.html"));
    this->_defaultErrorPages.insert(std::pair<int, std::string>(HTTP_PAYLOAD_TOO_LARGE, "default-413.html"));
    this->_defaultErrorPages.insert(std::pair<int, std::string>(HTTP_INTERNAL_SERVER_ERROR, "default-500.html"));
    this->_defaultErrorPages.insert(std::pair<int, std::string>(HTTP_VERSION_NOT_SUPPORTED, "default-505.html"));
}

const Rewrite &Config::getRewrite() const
{
    return this->_rewrite;
}

const std::string &Config::getRoot() const
{
    return this->_root;
}

const std::string &Config::getPath() const
{
    return this->_path;
}

bool Config::hasErrorPage(int status) const
{
    if (this->_errorPages.count(status) == 0)
        return false;
    if (lib::isFileExist(this->_root + "/" + this->_errorPages.at(status)) == false)
        return false;
    return true;
}

std::string Config::getErrorPagePath(int status) const
{
    std::string path;

    if (this->hasErrorPage(status) == true)
    {
        path = this->_root + "/" + this->_errorPages.at(status);
        return (path);
    }
    path = ROOT_DEFAULT_ERROR_PAGES + this->_defaultErrorPages.at(status);
    return path;
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

bool Config::hasRewrite() const
{
    if (this->_rewrite.status != -1)
        return true;
    return false;
}

const std::vector<std::string> &Config::getIndexes() const
{
    return this->_indexes;
}

const std::vector<std::string> &Config::getServerNames() const
{
    return this->_serverNames;
}

bool Config::hasMethod(const std::string &method)
{
    if (this->_allowedHttpMethods.size() == 0)
        return true;

    std::vector<std::string>::iterator it = std::find(this->_allowedHttpMethods.begin(), this->_allowedHttpMethods.end(), method);

    if (it != this->_allowedHttpMethods.end())
        return true;
    return false;
}

bool Config::hasCGI(const std::string &path) const
{
    if (this->_cgi.size() == 0)
        return false;
    const std::string extension = lib::extractExtension(path);
    if (extension.empty())
        return false;
    if (this->_cgi.count(extension) == 0)
        return false;
    return true;
}

const std::string &Config::getCGIPath(const std::string &path) const
{
    const std::string extension = lib::extractExtension(path);

    return this->_cgi.at(extension);
}

Config::~Config()
{
}
