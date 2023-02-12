#include "HttpRequestHandler.hpp"

/**
 * public
 */

std::string HttpRequestHandler::getFileContentType(const std::string &path)
{
    std::size_t found;
    std::string key;

    found = path.find_last_of(".");
    key = path.substr(found + 1);
    if (ContentType::contentTypes.find(key) == ContentType::contentTypes.end())
        return std::string("");
    return ContentType::contentTypes.at(key);
}

void HttpRequestHandler::handleClientMaxBodySize(std::size_t clientMaxBodySize)
{
    if (this->getHttpParser().getReceivedBodySize() > clientMaxBodySize)
    {
        std::remove(this->_putFilename.c_str());
        throw HttpPayloadTooLargeException();
    }
}

HttpRequestHandler::HttpRequestHandler(int connectionRef, const sockaddr_in &remoteSin)
{
    this->_connectionRef = connectionRef;
    this->_requestReadTimeout = 60;
    this->_requestLastRead = ::time(NULL);
    this->setIsIdleStatus();
    this->_responseHttpStatus = -1;
    this->_responseContentLength = 0;
    this->_responseBytesSent = 0;
    this->_requestBodyOffset = 0;
    this->_cgiWriteEnd = -1;
    this->_remoteSin = remoteSin;
    this->setRemoteAddressIp();
}

int HttpRequestHandler::getConnectionRef(void) const
{
    return this->_connectionRef;
}

time_t HttpRequestHandler::getRequestLastRead(void) const
{
    return this->_requestLastRead;
}

int HttpRequestHandler::getRequestTimeout(void) const
{
    return this->_requestReadTimeout;
}

const HttpParser &HttpRequestHandler::getHttpParser(void) const
{
    return this->_httpParser;
}

bool HttpRequestHandler::isIdleStatus(void) const
{
    return this->_status == IsIdle;
}

void HttpRequestHandler::setIsIdleStatus(void)
{
    this->_status = IsIdle;
}

void HttpRequestHandler::setIsDoneStatus(void)
{
    this->_status = IsDone;
}

bool HttpRequestHandler::isDone(void) const
{
    return this->_status == IsDone;
}

void HttpRequestHandler::setIsWritingRequestBodyStatus(void)
{
    this->_status = IsWritingRequestBody;
}

bool HttpRequestHandler::isWritingRequestBody(void) const
{
    return this->_status == IsWritingRequestBody;
}

void HttpRequestHandler::setIsWritingResponseBodyStatus(void)
{
    this->_status = IsWritingResponseBody;
}

bool HttpRequestHandler::isWritingResponseBody(void) const
{
    return this->_status == IsWritingResponseBody;
}

bool HttpRequestHandler::isTimeout(void) const
{
    return this->getResponseHttpStatus() == HTTP_REQUEST_TIMEOUT;
}

void HttpRequestHandler::setResponseContentLength(off_t contentLength)
{
    this->_responseContentLength = contentLength;
}

off_t HttpRequestHandler::getResponseContentLength(void) const
{
    return this->_responseContentLength;
}

void HttpRequestHandler::setResponseHttpStatus(int httpStatus)
{
    this->_responseHttpStatus = httpStatus;
}

int HttpRequestHandler::getResponseHttpStatus(void) const
{
    return this->_responseHttpStatus;
}

void HttpRequestHandler::setRemoteAddressIp(void)
{
    char str[INET_ADDRSTRLEN];

    (void)::inet_ntop(AF_INET, &this->_remoteSin.sin_addr, str, INET_ADDRSTRLEN);

    this->_remoteAddressIp.assign(str);
}

void HttpRequestHandler::read(void)
{
    ssize_t receivedBytes;
    char buffer[4096];
    const int bufferMaxSize = 4096;

    if (this->_httpParser.isRequestReady())
        return;

    receivedBytes = ::recv(this->_connectionRef, buffer, bufferMaxSize, 0);

    if (receivedBytes <= 0)
        return;

    this->_requestLastRead = ::time(NULL);

    this->_httpParser.append(buffer, receivedBytes);
    this->_httpParser.process();
}

void HttpRequestHandler::resumeWritingRequestBody(std::size_t clientMaxBodySize)
{
    const ArrayBuffer &body = this->_httpParser.getBody();
    ssize_t writtentSize;
    bool isDone;

    isDone = false;

    this->handleClientMaxBodySize(clientMaxBodySize);

    if (this->_httpParser.getBody().size() != 0)
    {
        if (this->_putFile.is_open())
        {
            std::copy(body.begin(), body.end(), std::ostream_iterator<unsigned char>(this->_putFile));
            this->_requestBodyOffset += body.size();
        }
        else
        {
            writtentSize = ::write(this->_cgiWriteEnd, &body[0], body.size());
            this->_requestBodyOffset += body.size();

            if (writtentSize < 0 || this->_requestBodyOffset == this->_httpParser.getContentLength())
                isDone = true;
        }
        this->_httpParser.clearBody();
    }

    if (this->_httpParser.hasHeader("transfer-encoding"))
        isDone = this->_httpParser.isRequestReady();
    else
        isDone = this->_requestBodyOffset == this->_httpParser.getContentLength();

    if (isDone)
    {
        if (this->_cgiWriteEnd != -1)
        {
            ::close(this->_cgiWriteEnd);
            this->_cgiWriteEnd = -1;
        }
        else if (this->_putFile.is_open())
        {
            this->_putFile.close();
            this->sendStatus(HTTP_CREATED, HTTP_CREATED_MESSAGE);
        }

        this->setIsDoneStatus();
    }
}

void HttpRequestHandler::resumeWritingResponseBody(void)
{
    char readBuffer[4096] = {0};
    std::streamsize size;
    ssize_t sentBytes;

    if (this->_restSendBuffer.empty())
    {

        std::istream &i = this->_staticFile.read(readBuffer, 4096);
        size = i.gcount();

        if (size == 0)
        {
            this->setIsDoneStatus();
            return;
        }

        sentBytes = ::send(this->getConnectionRef(), readBuffer, size, 0);

        if (sentBytes >= 0 && sentBytes < size)
            this->_restSendBuffer.insert(this->_restSendBuffer.end(), readBuffer + sentBytes, readBuffer + size);
    }
    else
    {
        sentBytes = ::send(this->getConnectionRef(), &this->_restSendBuffer[0], this->_restSendBuffer.size(), 0);
        if (sentBytes >= 0)
            this->_restSendBuffer.erase(this->_restSendBuffer.begin(), this->_restSendBuffer.begin() + sentBytes);
    }

    this->_responseBytesSent += sentBytes;

    if (sentBytes < 0 || this->_responseBytesSent == this->getResponseContentLength())
        this->setIsDoneStatus();
}

void HttpRequestHandler::executeGet(const std::string &path, int httpStatus, const std::string &statusMessage)
{
    try
    {
        std::stringstream headers;
        const FileStat &stat = FileStat::open(path);

        if (stat.isFolder())
            throw HttpForbiddenException();

        this->_responseContentLength = stat.getSize();
        this->setResponseHttpStatus(httpStatus);
        this->setIsWritingResponseBodyStatus();
        this->setResponseContentLength(stat.getSize());
        std::string contentType;

        contentType = this->getFileContentType(path);
        headers << "HTTP/1.1 " << httpStatus << " " << statusMessage << CRLF;
        headers << "Content-Length: " << stat.getSize() << CRLF;
        if (contentType.empty() == false)
            headers << "Content-Type: " << contentType << CRLF;
        headers << CRLF;

        const std::string &headersString = headers.str();
        if (::send(this->_connectionRef, headersString.c_str(), headersString.length(), 0) <= 0)
        {
            this->setIsDoneStatus();
            return;
        }

        this->sendFile(path);
    }
    catch (const FileStat::FileStatException &e)
    {
        throw HttpNotFoundException();
    }
}

void HttpRequestHandler::executeDelete(const std::string &path)
{
    try
    {
        std::stringstream headers;

        if (this->hasDeleted(path) == true)
        {
            headers << "HTTP/1.1 " << HTTP_NO_CONTENT << " " << HTTP_NO_CONTENT_MESSAGE << CRLF;
            headers << CRLF;
            const std::string &headersString = headers.str();
            ::send(this->_connectionRef, headersString.c_str(), headersString.length(), 0);
            this->setResponseHttpStatus(HTTP_NO_CONTENT);
            this->setIsDoneStatus();
        }
        else
            throw HttpForbiddenException();
    }
    catch (const FileStat::FileStatException &e)
    {
        throw HttpNotFoundException();
    }
}

bool HttpRequestHandler::hasDeleted(const std::string &path)
{
    const FileStat &stat = FileStat::open(path);
    int result;

    if (lib::isFileExist(path) == false)
        throw HttpNotFoundException();
    if (stat.isFolder() == false)
        result = std::remove(path.c_str());
    else
        result = lib::removeDirectory(path.c_str());
    if (result == 0)
        return true;
    return false;
}

void HttpRequestHandler::executePut(const std::string &path, std::size_t clientMaxBodySize)
{
    const ArrayBuffer &body = this->_httpParser.getBody();
    const std::size_t bodySize = body.size();
    bool isDone;

    if (!this->getHttpParser().hasHeader("content-length") && !this->getHttpParser().hasHeader("transfer-encoding"))
        throw HttpBadRequestException();

    this->_putFilename.assign(path);
    this->_putFile.open(path.c_str());

    if (!this->_putFile.good())
        throw HttpForbiddenException();

    this->handleClientMaxBodySize(clientMaxBodySize);

    this->setIsWritingRequestBodyStatus();
    this->setResponseHttpStatus(HTTP_CREATED);

    std::copy(body.begin(), body.end(), std::ostream_iterator<unsigned char>(this->_putFile));
    this->_httpParser.clearBody();

    isDone = false;

    if (this->_httpParser.hasHeader("transfer-encoding"))
        isDone = this->_httpParser.isRequestReady();
    else
        isDone = bodySize == this->_httpParser.getContentLength();

    if (isDone)
    {
        this->_putFile.close();
        this->sendStatus(HTTP_CREATED, HTTP_CREATED_MESSAGE);
        this->setIsDoneStatus();
    }
}

bool HttpRequestHandler::sendStatus(int httpStatus, const std::string &statusMessage)
{
    std::stringstream message;

    message << "HTTP/1.1 " << httpStatus << " " << statusMessage << CRLF CRLF;
    const std::string &statusString = message.str();
    ssize_t sentBytes = ::send(this->_connectionRef, statusString.c_str(), statusString.length(), 0);

    return sentBytes != -1;
}

void HttpRequestHandler::serveIndexFile(const std::string &path, const Config &config)
{
    std::stringstream headers;
    FileStat stat;
    std::string indexPath;
    const std::vector<std::string> &indexs = config.getIndexes();
    bool autoIndex;

    try
    {
        size_t i;
        for (i = 0; i < indexs.size(); i++)
        {
            indexPath = path + indexs[i];
            if (lib::isFileExist(indexPath) == true)
            {
                stat = FileStat::open(indexPath);
                if (stat.isFolder() == true)
                    throw HttpForbiddenException();
                break;
            }
        }
        if (i == indexs.size())
        {
            if (this->_httpParser.getMethod() != HTTP_GET)
                throw HttpMethodNotAllowedException();
            autoIndex = config.getAutoIndex();
            if (autoIndex == true)
            {
                this->setResponseHttpStatus(HTTP_OK);
                this->setIsWritingResponseBodyStatus();
                this->setResponseContentLength(this->directoryListing(path));
                this->setIsDoneStatus();
            }
            else
                throw HttpForbiddenException();
        }
        else
        {
            if (config.hasCGI(indexPath) == true)
                this->handleCGI(indexPath, config.getCGIPath(indexPath));
            else
            {
                if (this->_httpParser.getMethod() != HTTP_GET)
                    throw HttpMethodNotAllowedException();
                this->executeGet(indexPath, HTTP_OK, HTTP_OK_MESSAGE);
            }
        }
    }
    catch (const FileStat::FileStatException &e)
    {
        throw HttpInternalServerErrorException();
    }
}

off_t HttpRequestHandler::directoryListing(const std::string &dirPath)
{
    struct dirent **entity;
    std::stringstream headers;
    std::stringstream body;
    std::string stringBody;
    int fileNumber;

    body << "<html><head><title>Directory listing for</title></head><body><ul>";
    fileNumber = scandir(dirPath.c_str(), &entity, NULL, alphasort);
    if (fileNumber == -1)
        throw HttpForbiddenException();
    for (int i = 0; i < fileNumber; i++)
    {
        if (entity[i]->d_type == DT_DIR)
        {
            body << "<li><a href=" << entity[i]->d_name << "/"
                 << ">" << entity[i]->d_name << "/"
                 << "</a></li>" << std::endl;
        }
        else
            body << "<li><a href=" << entity[i]->d_name << ">" << entity[i]->d_name << "</a></li>" << std::endl;
        free(entity[i]);
    }
    free(entity);
    body << "</ul></body>";
    stringBody = body.str();
    headers << "HTTP/1.1 " << HTTP_OK << " " << HTTP_OK_MESSAGE << CRLF;
    headers << "Content-Length: " << stringBody.length() << CRLF;
    headers << "Content-Type: text/html" << CRLF;
    headers << CRLF;

    const std::string &response = headers.str() + stringBody;
    ::send(this->_connectionRef, response.c_str(), response.length(), 0);
    return stringBody.length();
}

void HttpRequestHandler::sendFile(const std::string &path)
{
    char readBuffer[4096];
    ssize_t sentBytes;
    std::streamsize size;

    this->_staticFile.open(path.c_str());

    std::istream &i = this->_staticFile.read(readBuffer, 4096);
    size = i.gcount();
    if (size > 0)
    {
        sentBytes = ::send(this->getConnectionRef(), readBuffer, size, 0);

        if (sentBytes <= 0)
            this->setIsDoneStatus();
        else
        {
            if (sentBytes < size)
                this->_restSendBuffer.insert(this->_restSendBuffer.begin(), readBuffer + sentBytes, readBuffer + size);
            this->_responseBytesSent = sentBytes;
        }
    }

    if (size == 0 || this->_responseBytesSent == this->getResponseContentLength())
        this->setIsDoneStatus();
}

void HttpRequestHandler::setCGIEnv(const std::string &path, const HttpParser &httpParser) const
{
    const Url &requestTarget = httpParser.getRequestTarget();
    std::string requestUri = requestTarget.path;
    StringStringMap::const_iterator it = this->_httpParser.getHeaders().begin();
    std::string headerName;

    if (!requestTarget.queryString.empty())
        requestUri += std::string("?") + requestTarget.queryString;

    ::setenv("QUERY_STRING", requestTarget.queryString.c_str(), 1);
    ::setenv("REQUEST_METHOD", httpParser.getMethod().c_str(), 1);
    ::setenv("PATH_INFO", path.c_str(), 1);
    ::setenv("SCRIPT_FILENAME", path.c_str(), 1);
    ::setenv("SCRIPT_NAME", requestTarget.path.c_str(), 1);
    ::setenv("REQUEST_URI", requestUri.c_str(), 1);
    ::setenv("DOCUMENT_URI", requestTarget.path.c_str(), 1);
    ::setenv("REDIRECT_STATUS", "200", 1);
    ::setenv("REMOTE_ADDR", this->_remoteAddressIp.c_str(), 1);
    ::setenv("REMOTE_PORT", lib::toString(this->_remoteSin.sin_port).c_str(), 1);

    if (httpParser.hasHeader("transfer-encoding"))
        ::setenv("CONTENT_LENGTH", "-1", 1);
    else if (httpParser.hasHeader("content-length"))
        ::setenv("CONTENT_LENGTH", httpParser.getHeader("content-length").c_str(), 1);

    if (httpParser.hasHeader("content-type"))
        ::setenv("CONTENT_TYPE", httpParser.getHeader("content-type").c_str(), 1);

    for (; it != this->_httpParser._headers.end(); ++it)
    {
        headerName = "HTTP_" + it->first;
        lib::transform(headerName.begin(), headerName.end(), lib::replaceDashWithUnderscore);
        lib::transform(headerName.begin(), headerName.end(), std::toupper);
        ::setenv(headerName.c_str(), it->second.c_str(), 1);
    }
}

void HttpRequestHandler::runCGI(const std::string &path, const std::string &cgiBinPath)
{
    extern char **environ;
    const HttpParser &httpParser = this->getHttpParser();
    pid_t pid;

    int fds[2];
    char *argv[] = {
        const_cast<char *>(cgiBinPath.c_str()),
        const_cast<char *>(path.c_str()),
        NULL,
    };

    if (httpParser.isRequestMethodHasBody())
    {
        if (::pipe(fds) == -1)
            throw HttpInternalServerErrorException();
        this->_cgiWriteEnd = fds[1];
        std::cerr << this->_cgiWriteEnd << std::endl;
    }

    pid = ::fork();

    if (pid == -1)
    {
        if (this->_cgiWriteEnd != -1)
        {
            ::close(fds[0]);
            ::close(fds[1]);
            this->_cgiWriteEnd = -1;
        }
        throw HttpInternalServerErrorException();
    }

    if (pid == 0)
    {
        if (httpParser.isRequestMethodHasBody())
        {
            if (::dup2(fds[0], 0) == -1)
                exit(1);
            ::close(fds[0]);
            ::close(fds[1]);
        }

        this->setCGIEnv(path, httpParser);

        if (::dup2(this->getConnectionRef(), 1) == -1)
            exit(1);

        if (::close(this->getConnectionRef()) == -1)
            exit(1);

        if (::send(1, "HTTP/1.1 200 OK" CRLF, 17, 0) <= 0)
            exit(1);

        ::execve(cgiBinPath.c_str(), argv, environ);
        ::exit(1);
    }

    if (httpParser.isRequestMethodHasBody())
        ::close(fds[0]);
}

void HttpRequestHandler::handleCGI(const std::string &path, const std::string &pathCGI)
{
    if (!lib::isFileExist(path))
        throw HttpNotFoundException();

    this->setResponseHttpStatus(HTTP_OK);

    if (this->getHttpParser().isRequestMethodHasBody())
        this->setIsWritingRequestBodyStatus();
    else
        this->setIsDoneStatus();

    this->runCGI(path, pathCGI);
}

void HttpRequestHandler::logAccess(void) const
{
    char dateString[27];

    const time_t nowTimestamp = ::time(NULL);

    lib::formatTime(dateString, 27, "%d/%b/%Y:%X %z", nowTimestamp);

    std::cout << this->_remoteAddressIp << " - ["
              << dateString
              << "] \""
              << this->_httpParser.getMethod()
              << " "
              << this->_httpParser.getRequestTarget().origin
              << "\" "
              << this->getResponseHttpStatus()
              << " "
              << this->getResponseContentLength() << std::endl;
}

void HttpRequestHandler::rewrite(const Rewrite &rewrite)
{
    std::stringstream headers;
    std::string rewriteType;
    int status;

    status = rewrite.status;
    if (status == HTTP_TEMPORARY_REDIRECT)
        rewriteType = HTTP_TEMPORARY_REDIRECT_MESSAGE;
    else if (status == HTTP_PERMANENT_REDIRECT)
        rewriteType = HTTP_PERMANENT_REDIRECT_MESSAGE;

    headers << "HTTP/1.1 " << status << " " << rewriteType << CRLF;
    headers << "Location: " << rewrite.url << CRLF;
    headers << CRLF;

    const std::string &headersContent = headers.str();
    ::send(this->_connectionRef, headersContent.c_str(), headersContent.size(), 0);
    this->setIsDoneStatus();
}

HttpRequestHandler::~HttpRequestHandler()
{
}
