#pragma once

#include <map>
#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <cctype>
#include <cstdlib>

#include "Url.hpp"
#include "lib.hpp"

#define CRLF "\r\n"
#define CRLF_LEN 2
// #define CRLF "\n"
// #define CRLF_LEN 1
#define READING_REQUEST_LINE_STATUS 1
#define READING_HEADERS_STATUS 2
#define READING_BODY_STATUS 3
#define REQUEST_READY_STATUS 4

#define HTTP_GET "GET"
#define HTTP_HEAD "HEAD"
#define HTTP_POST "POST"
#define HTTP_PUT "PUT"
#define HTTP_DELETE "DELETE"
#define HTTP_OPTIONS "OPTIONS"

class HttpRequestHandler
{
private:
    int _connectionRef;
    std::map<std::string, std::string> _headers;
    std::string _method;
    Url _requestTarget;
    std::map<std::string, std::string> _queryParams;
    std::string _httpVersion;
    std::string _body;
    std::string _buffer;
    std::size_t _contentLength;
    int _status;

    void parseRequestLine(const std::string &line);

    bool isHeaderFieldName(const std::string &name);

    void parseRequestHeader(const std::string &line);

    void setMethod(const std::string &method);

    void setRequestTarget(const std::string &requestTarget);

    void setHttpVersion(const std::string &httpVersion);

public:
    HttpRequestHandler(int connectionRef);

    int getConnectionRef(void) const;

    bool isRequestReady(void) const;

    void read(void);

    ~HttpRequestHandler();

    /**
     * exceptions
     */
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
        HttpBadRequestException(void);
        virtual ~HttpBadRequestException() throw();
    };
};
