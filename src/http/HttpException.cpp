#include "HttpException.hpp"

/**
 * HttpRequestException
 */

int AHttpRequestException::getHttpStatus(void) const throw()
{
    return this->_httpStatus;
}

const char *AHttpRequestException::what() const throw()
{
    return this->_message.c_str();
}

AHttpRequestException::~AHttpRequestException() throw()
{
}

/**
 * HttpBadRequestException
 */
HttpBadRequestException::HttpBadRequestException(void) throw()
{
    this->_httpStatus = HTTP_BAD_REQUEST;
    this->_message = HTTP_BAD_REQUEST_MESSAGE;
}

HttpBadRequestException::~HttpBadRequestException() throw()
{
}

/**
 * HttpNotFoundException
 */
HttpNotFoundException::HttpNotFoundException(void) throw()
{
    this->_httpStatus = HTTP_NOT_FOUND;
    this->_message = HTTP_NOT_FOUND_MESSAGE;
}

HttpNotFoundException::~HttpNotFoundException() throw()
{
}

/**
 * HttpRequestTimeoutException
 */
HttpRequestTimeoutException::HttpRequestTimeoutException(void) throw()
{
    this->_httpStatus = HTTP_REQUEST_TIMEOUT;
    this->_message = HTTP_REQUEST_TIMEOUT_MESSAGE;
}

HttpRequestTimeoutException::~HttpRequestTimeoutException() throw()
{
}

/**
 * HttpForbidden
 */

HttpForbiddenException::HttpForbiddenException(void) throw()
{
    this->_httpStatus = HTTP_FORBIDDEN;
    this->_message = HTTP_FORBIDDEN_MESSAGE;
}

HttpForbiddenException::~HttpForbiddenException() throw()
{
}

HttpInternalServerErrorException::HttpInternalServerErrorException(void) throw()
{
    this->_httpStatus = HTTP_INTERNAL_SERVER_ERROR;
    this->_message = HTTP_INTERNAL_SERVER_ERROR_MESSAGE;
}

HttpInternalServerErrorException::~HttpInternalServerErrorException() throw()
{
}

HttpMethodNotAllowedException::HttpMethodNotAllowedException(void) throw()
{
    this->_httpStatus = HTTP_METHOD_NOT_ALLOWED;
    this->_message = HTTP_METHOD_NOT_ALLOWED_MESSAGE;
}

HttpMethodNotAllowedException::~HttpMethodNotAllowedException() throw()
{
}
