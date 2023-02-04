#pragma once

#include <exception>
#include <string>

class ConfigException : public std::exception
{
private:
    std::string _message;

public:
    ConfigException(const std::string &message);
    virtual const char *what() const throw();
    virtual ~ConfigException() throw();
};
