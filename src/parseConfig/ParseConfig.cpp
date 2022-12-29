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
        std::cout << ParseConfig::parseRoot(line);
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

// int ParseConfig::parseClientMaxBodySize(std::string &line)
// {

//     return 0;
// }

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