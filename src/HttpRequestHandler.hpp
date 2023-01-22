#pragma once

#include <map>
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <cctype>
#include <cstdlib>
#include <sys/stat.h>
#include <vector>
#include <dirent.h>

#include "Url.hpp"
#include "lib.hpp"
#include "FileStat.hpp"
#include "httpStatus.hpp"

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
    int _requestReadTimeout;
    time_t _requestLastRead;

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

    const std::string &getMethod(void) const;

    const Url &getRequestTarget(void) const;

    time_t getRequestLastRead(void) const;

    int getRequestTimeout(void) const;

    void read(void);

    const char *getRequestHost() const;

    /**
     * serve static files using this->_requestTarget
     */
    off_t serveStatic(const std::string &root, int httpStatus, const std::string &statusMessage);
    off_t serveIndexFile(const std::string &root, std::vector<std::string> indexs, bool autoIndex);

    off_t directoryListing(const std::string &dirPath);

    void sendFile(const std::string &path) const;
//
    void logAccess(int httpStatus, std::size_t contentLength,const std::string &remoteAddress) const;

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
};
