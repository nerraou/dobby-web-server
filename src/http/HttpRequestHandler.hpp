#pragma once

#include <map>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <cctype>
#include <cstdlib>
#include <sys/stat.h>
#include <vector>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "HttpException.hpp"
#include "Url.hpp"
#include "lib.hpp"
#include "FileStat.hpp"
#include "httpStatus.hpp"
#include "HttpParser.hpp"
#include "ContentType.hpp"

#define CRLF "\r\n"

#define HTTP_GET "GET"
#define HTTP_POST "POST"
#define HTTP_PUT "PUT"
#define HTTP_DELETE "DELETE"

class HttpRequestHandler
{
public:
    enum RequestStatus
    {
        IsIdle = 0,
        IsWritingRequestBody = 1,
        IsWritingResponseBody = 2,
        IsDone = 3,
    };

private:
    int _connectionRef;
    int _requestReadTimeout;
    int _responseHttpStatus;
    int _cgiWriteEnd;
    RequestStatus _status;
    time_t _requestLastRead;
    HttpParser _httpParser;
    std::ifstream _staticFile;
    ArrayBuffer _restSendBuffer;
    off_t _responseBytesSent;
    off_t _responseContentLength;
    std::size_t _requestBodyOffset;
    sockaddr_in _remoteSin;
    std::string _remoteAddressIp;

    void setRemoteAddressIp(void);

    std::string getFileContentType(const std::string &path);

public:
    HttpRequestHandler(int connectionRef, const sockaddr_in &remoteSin);

    bool isIdleStatus(void) const;

    void setIsIdleStatus(void);

    void setIsDoneStatus(void);

    bool isDone(void) const;

    void setIsWritingRequestBodyStatus(void);

    bool isWritingRequestBody(void) const;

    void setIsWritingResponseBodyStatus(void);

    bool isWritingResponseBody(void) const;

    bool isTimeout(void) const;

    int getConnectionRef(void) const;

    time_t getRequestLastRead(void) const;

    int getRequestTimeout(void) const;

    void setResponseContentLength(off_t contentLength);

    off_t getResponseContentLength(void) const;

    void setResponseHttpStatus(int httpStatus);

    int getResponseHttpStatus(void) const;

    void read(void);

    void resumeWritingRequestBody(void);

    void resumeWritingResponseBody(void);

    const HttpParser &getHttpParser(void) const;

    /**
     * serve static files using this->_requestTarget
     */
    void serveStatic(const std::string &root, int httpStatus, const std::string &statusMessage);
    void serveIndexFile(const std::string &root, std::vector<std::string> indexs, bool autoIndex);

    off_t directoryListing(const std::string &dirPath);

    void sendFile(const std::string &path);

    void setCGIEnv(const std::string &path, const HttpParser &httpParser) const;
    void runCGI(const std::string &path, const std::string &cgiBinPath);

    //
    void logAccess(void) const;

    ~HttpRequestHandler();
};
