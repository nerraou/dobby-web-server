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

HttpRequestHandler::HttpRequestHandler(int connectionRef, const sockaddr_in &remoteSin)
{
    this->_connectionRef = connectionRef;
    this->_requestReadTimeout = 60;
    this->_requestLastRead = ::time(NULL);
    this->setIsIdleStatus();
    this->_responseContentLength = 0;
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
    ArrayBuffer buffer;

    if (this->_httpParser.isRequestReady())
        return;

    buffer.reserve(4096);

    receivedBytes = ::recv(this->_connectionRef, &buffer[0], buffer.capacity(), 0);

    if (receivedBytes <= 0)
        return;

    this->_requestLastRead = ::time(NULL);

    this->_httpParser.append(buffer.begin(), buffer.begin() + receivedBytes);
    this->_httpParser.process();
}

void HttpRequestHandler::resumeWritingRequestBody(void)
{
    const ArrayBuffer &body = this->_httpParser.getBody();
    ssize_t writtentSize;

    if (this->_httpParser.getBody().size() != 0)
    {
        writtentSize = ::write(this->_cgiWriteEnd, &body[0], body.size());
        this->_requestBodyOffset += body.size();
        this->_httpParser.clearBody();

        if (writtentSize <= 0 || this->_requestBodyOffset == this->_httpParser.getContentLength())
        {
            ::close(this->_cgiWriteEnd);
            this->_cgiWriteEnd = -1;
            this->setIsDoneStatus();
        }
    }
}

void HttpRequestHandler::resumeWritingResponseBody(void)
{
    char readBuffer[4096];

    std::istream &i = this->_staticFile.read(readBuffer, 4096);
    if (i.gcount() == 0)
    {
        this->setIsDoneStatus();
        return;
    }

    if (::send(this->getConnectionRef(), readBuffer, i.gcount(), 0) <= 0)
    {
        this->setIsDoneStatus();
    }
}

void HttpRequestHandler::serveStatic(const std::string &path, int httpStatus, const std::string &statusMessage)
{
    try
    {
        std::stringstream headers;
        const FileStat &stat = FileStat::open(path);

        if (stat.isFolder())
            throw HttpForbiddenException();

        this->_responseContentLength = stat.getSize();

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
            throw HttpInternalServerErrorException();

        this->sendFile(path);
    }
    catch (const FileStat::FileStatException &e)
    {
        throw HttpNotFoundException();
    }
}

void HttpRequestHandler::serveIndexFile(const std::string &path, std::vector<std::string> indexs, bool autoIndex)
{
    std::stringstream headers;
    FileStat stat;
    std::string indexPath;

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
            if (autoIndex == true)
                this->setResponseContentLength(this->directoryListing(path));
            else
                throw HttpForbiddenException();
        }
        else
            this->serveStatic(indexPath, HTTP_OK, HTTP_OK_MESSAGE);
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
    headers << CRLF;

    const std::string &response = headers.str() + stringBody;
    if (::send(this->_connectionRef, response.c_str(), response.length(), 0) <= 0)
        this->setIsDoneStatus();
    return stringBody.length();
}

void HttpRequestHandler::sendFile(const std::string &path)
{
    char readBuffer[512];

    this->_staticFile.open(path.c_str());

    std::istream &i = this->_staticFile.read(readBuffer, 512);
    if (i.gcount() == 0)
    {
        this->setIsDoneStatus();
        return;
    }

    if (::send(this->getConnectionRef(), readBuffer, i.gcount(), 0) <= 0)
        this->setIsDoneStatus();
}

void HttpRequestHandler::setCGIEnv(const std::string &path, const HttpParser &httpParser) const
{
    const Url &requestTarget = httpParser.getRequestTarget();
    std::string requestUri = requestTarget.path;

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

    if (httpParser.hasHeader("content-type"))
        ::setenv("CONTENT_TYPE", httpParser.getHeader("content-type").c_str(), 1);
    if (httpParser.hasHeader("content-length"))
        ::setenv("CONTENT_LENGTH", httpParser.getHeader("content-length").c_str(), 1);
}

void HttpRequestHandler::runCGI(const std::string &path, const std::string &cgiBinPath)
{
    extern char **environ;
    const HttpParser &httpParser = this->getHttpParser();

    int fds[2];
    char *argv[] = {
        (char *)cgiBinPath.c_str(),
        NULL,
    };
    (void)argv;
    (void)(environ);

    if (::pipe(fds) == -1)
        throw HttpInternalServerErrorException();

    this->_cgiWriteEnd = fds[1];

    if (::fork() == 0)
    {
        ::dup2(fds[0], 0);
        ::close(fds[0]);
        ::close(fds[1]);

        this->setCGIEnv(path, httpParser);

        ::dup2(this->getConnectionRef(), 1);

        if (::send(1, "HTTP/1.1 200 OK" CRLF, 17, 0) <= 0)
            exit(1);

        ::execve(cgiBinPath.c_str(), argv, environ);
        ::exit(1);
    }
    ::close(fds[0]);
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

void HttpRequestHandler::rewrite(const Config &config)
{
    std::stringstream headers;
    std::string rewriteType;
    int status;

    status = config.getRewrite().status;
    if (status == 307)
        rewriteType = "Temporary Redirect";
    else if (status == 308)
        rewriteType = "Permanent Redirect";

    headers << "HTTP/1.1 " << status << " " << rewriteType << CRLF;
    headers << "Location: " << config.getRewrite().url << CRLF;
    headers << CRLF;
    std::cout << "send headers\n";
    (::send(this->_connectionRef, headers.str().c_str(), headers.str().size(), 0));
    this->setIsDoneStatus();
}

HttpRequestHandler::~HttpRequestHandler()
{
}
