#include "ConfigException.hpp"

ConfigException::ConfigException(const std::string &message)
{
    this->_message = message;
}

const char *ConfigException::what() const throw()
{
    return this->_message.c_str();
}

ConfigException::~ConfigException() throw()
{
}
