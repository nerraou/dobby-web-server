#pragma once

#include <string>
#include <exception>
#include "httpStatus.hpp"

class AHttpRequestException : public std::exception
{
protected:
    int _httpStatus;
    std::string _message;

public:
    int getHttpStatus(void) const throw();
    virtual const char *what() const throw();
    virtual ~AHttpRequestException() throw();
};

class HttpBadRequestException : public AHttpRequestException
{
public:
    HttpBadRequestException(void) throw();
    virtual ~HttpBadRequestException() throw();
};

class HttpNotFoundException : public AHttpRequestException
{
public:
    HttpNotFoundException(void) throw();
    virtual ~HttpNotFoundException() throw();
};

class HttpRequestTimeoutException : public AHttpRequestException
{
public:
    HttpRequestTimeoutException(void) throw();
    virtual ~HttpRequestTimeoutException() throw();
};
class HttpForbiddenException : public AHttpRequestException
{
public:
    HttpForbiddenException(void) throw();
    virtual ~HttpForbiddenException() throw();
};

class HttpInternalServerErrorException : public AHttpRequestException
{
public:
    HttpInternalServerErrorException(void) throw();
    virtual ~HttpInternalServerErrorException() throw();
};
