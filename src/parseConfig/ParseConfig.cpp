#include "ParseConfig.hpp"

ParseConfig::ParseConfig()
{
}

ParseConfig::~ParseConfig()
{
}

void ParseConfig::parse(std::string configPath)
{
    std::ifstream file(configPath, std::ifstream::in);
    std::string line;

    if (file.is_open() == false)
    {
        throw(ParseConfig::FileException());
    }
    while (std::getline(file, line))
    {
        std::cout << ParseConfig::parseListen(line);
    }
}

std::string ParseConfig::parseRoot(std::string &line)
{
    std::string root;

    root = lib::trim(line);
    if (root.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad root!"));
    return (root);
}

size_t ParseConfig::convertToUnit(int size, const char unit)
{
    if (unit == 'm')
        return size * 1000000;
    else
        return size * 1000;
}

size_t ParseConfig::checkConvertUnit(int size, const char *str)
{
    int len;
    int unit;

    if (str[0] == '\0')
        return size;
    len = ::strlen(str);
    if (len > 1)
        return (0);
    unit = tolower(str[0]);
    if ((char)unit == 'm' || (char)unit == 'k')
    {
        return convertToUnit(size, unit);
    }
    return (0);
}

size_t ParseConfig::parseClientMaxBodySize(std::string &line)
{
    size_t size;
    int num;
    char *pEnd;

    line = lib::trim(line);
    if (line.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad max body size!"));
    num = (int)std::strtol(line.c_str(), &pEnd, 10);
    if (num <= 0)
        throw(ParseConfig::ParseConfigException("Error Bad max body size!"));
    size = ParseConfig::checkConvertUnit(num, pEnd);
    if (size == 0)
        throw(ParseConfig::ParseConfigException("Error Bad max body size unit!"));
    return size;
}

int ParseConfig::parseListen(std::string &line)
{
    int listen;

    char *pEnd;

    line = lib::trim(line);
    if (line.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad Listen!"));
    listen = (int)std::strtol(line.c_str(), &pEnd, 10);
    if (listen <= 0 || listen > 65535 || pEnd[0] != '\0')
        throw(ParseConfig::ParseConfigException("Error Bad Listen!"));
    return listen;
}

bool ParseConfig::parseAutoIndex(std::string &line)
{
    line = lib::trim(line);

    if (std::strcmp(line.c_str(), "on") == 0)
        return (true);
    else if (std::strcmp(line.c_str(), "off") == 0)
        return false;
    else
        throw(ParseConfig::ParseConfigException("Error Bad AutoIndex"));
}

std::vector<std::string> ParseConfig::parseIndex(std::string &line)
{
    line = lib::trim(line);
    if (line.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad Index"));
    return lib::split(line);
}

std::vector<std::string> ParseConfig::parseErrorPage(std::string &line)
{
    line = lib::trim(line);
    if (line.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad Error page"));
    return lib::split(line);
}

std::vector<std::string> ParseConfig::parseServerName(std::string &line)
{
    line = lib::trim(line);
    if (line.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad Server name"));
    return lib::split(line);
}

std::vector<std::string> ParseConfig::parseAcceptedHttpMethods(std::string &line)
{
    line = lib::trim(line);
    if (line.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad Http methods"));
    return lib::split(line);
}

std::string ParseConfig::parseRewrite(std::string &line)
{
    line = lib::trim(line);
    if (line.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad Rewrite"));
    return line;
}

ParseConfig::ParseConfigException::ParseConfigException(const std::string &message)
{
    this->_message = message;
}

const char *ParseConfig::ParseConfigException::what() const throw()
{
    return this->_message.c_str();
}

ParseConfig::ParseConfigException::~ParseConfigException() throw()
{
}

const char *ParseConfig::FileException::what() const throw()
{
    return ("Error while trying to open the file.");
}

ParseConfig::FileException::~FileException() throw()
{
}