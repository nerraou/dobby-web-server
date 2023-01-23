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

#include "HttpException.hpp"
#include "Url.hpp"
#include "lib.hpp"
#include "FileStat.hpp"
#include "httpStatus.hpp"
#include "HttpParser.hpp"

#define CRLF "\r\n"

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
    int _requestReadTimeout;
    time_t _requestLastRead;
    HttpParser _httpParser;

public:
    HttpRequestHandler(int connectionRef);

    int getConnectionRef(void) const;

    time_t getRequestLastRead(void) const;

    int getRequestTimeout(void) const;

    void read(void);

    const HttpParser &getHttpParser(void) const;

    /**
     * serve static files using this->_requestTarget
     */
    off_t serveStatic(const std::string &root, int httpStatus, const std::string &statusMessage);
    off_t serveIndexFile(const std::string &root, std::vector<std::string> indexs, bool autoIndex);

    off_t directoryListing(const std::string &dirPath);

    void sendFile(const std::string &path) const;
    //
    void logAccess(int httpStatus, std::size_t contentLength, const std::string &remoteAddress) const;

    ~HttpRequestHandler();
};
