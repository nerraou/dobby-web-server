#include "HttpRequestHandler.hpp"

/**
 * private
 */
void HttpRequestHandler::parseRequestLine(const std::string &line)
{
    std::size_t start;
    std::size_t end;

    end = line.find(' ');
    if (end == line.npos || line.at(end) != ' ')
        throw HttpBadRequestException();

    this->setMethod(line.substr(0, end));

    start = line.find_first_not_of(' ', end);

    if (start == line.npos)
        throw HttpBadRequestException();

    end = line.find_first_of(' ', start);

    if (end == line.npos)
        throw HttpBadRequestException();

    try
    {
        this->setRequestTarget(line.substr(start, end - start));
    }
    catch (const Url::ParseUrlException &e)
    {
        throw HttpBadRequestException();
    }

    this->setHttpVersion(lib::trim(line.c_str() + end));
}

bool HttpRequestHandler::isHeaderFieldName(const std::string &name)
{
    const char *set = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";

    if (name.empty())
        return (false);
    return name.find_first_not_of(set) == name.npos;
}

void HttpRequestHandler::parseRequestHeader(const std::string &line)
{
    std::size_t pos;
    std::string fieldName;
    std::string fieldValue;
    bool isFieldName;

    pos = line.find(':');
    if (pos == line.npos)
        throw HttpBadRequestException();
    fieldName = line.substr(0, pos);
    isFieldName = isHeaderFieldName(fieldName);

    if (!isFieldName)
        throw HttpBadRequestException();

    lib::transform(fieldName.begin(), fieldName.end(), std::tolower);
    this->_headers[fieldName] = lib::trim(line.c_str() + pos + 1);

    if (fieldName.compare("content-length") == 0)
        this->_contentLength = std::atoi(fieldValue.c_str());
}

void HttpRequestHandler::setMethod(const std::string &method)
{
    const std::string methods[] = {
        "GET",
        "HEAD",
        "POST",
        "PUT",
        "DELETE",
    };
    const int methodsCount = 5;

    for (int i = 0; i < methodsCount; i++)
    {
        if (method.compare(methods[i]) != 0)
        {
            this->_method = method;
            return;
        }
    }
    throw HttpBadRequestException();
}

void HttpRequestHandler::setRequestTarget(const std::string &requestTarget)
{
    this->_requestTarget = Url::parse(requestTarget);
}

void HttpRequestHandler::setHttpVersion(const std::string &httpVersion)
{
    if (httpVersion.compare("HTTP/1.1") != 0)
        throw HttpBadRequestException();
    this->_httpVersion = httpVersion;
}

/**
 * public
 */
HttpRequestHandler::HttpRequestHandler(int connectionRef)
{
    this->_connectionRef = connectionRef;
    this->_status = READING_REQUEST_LINE_STATUS;
    this->_requestReadTimeout = 5;
    this->_requestLastRead = ::time(NULL);
}

int HttpRequestHandler::getConnectionRef(void) const
{
    return this->_connectionRef;
}

bool HttpRequestHandler::isRequestReady(void) const
{
    return this->_status == REQUEST_READY_STATUS;
}

const std::string &HttpRequestHandler::getMethod(void) const
{
    return this->_method;
}

const Url &HttpRequestHandler::getRequestTarget(void) const
{
    return this->_requestTarget;
}

time_t HttpRequestHandler::getRequestLastRead(void) const
{
    return this->_requestLastRead;
}

int HttpRequestHandler::getRequestTimeout(void) const
{
    return this->_requestReadTimeout;
}

void HttpRequestHandler::read(void)
{
    ssize_t readedBytes;
    char buffer[512];
    bool hasCRLF;
    std::size_t crlfPosition;

    if (this->_status == REQUEST_READY_STATUS)
        return;

    readedBytes = ::recv(this->_connectionRef, buffer, 511, 0);
    if (readedBytes <= 0)
        return;

    this->_requestLastRead = ::time(NULL);

    buffer[readedBytes] = '\0';

    if (this->_status == READING_BODY_STATUS)
    {
        if (this->_body.length() < this->_contentLength)
            this->_body.append(buffer, this->_contentLength - this->_body.length());
        return;
    }

    this->_buffer.append(buffer);
    crlfPosition = this->_buffer.find(CRLF);
    hasCRLF = crlfPosition != this->_buffer.npos;

    while (hasCRLF)
    {
        if (this->_status == READING_REQUEST_LINE_STATUS)
        {
            this->parseRequestLine(this->_buffer.substr(0, crlfPosition));
            this->_status = READING_HEADERS_STATUS;
            this->_buffer = this->_buffer.substr(crlfPosition + CRLF_LEN);
        }
        else if (crlfPosition == 0)
        {
            this->_status = READING_BODY_STATUS;
            if (this->_method.compare(HTTP_GET) != 0)
                this->_body.append(this->_buffer.substr(CRLF_LEN));
            this->_buffer.clear();
            break;
        }
        else if (this->_status == READING_HEADERS_STATUS)
        {
            this->parseRequestHeader(this->_buffer.substr(0, crlfPosition));
            this->_buffer = this->_buffer.substr(crlfPosition + CRLF_LEN);
        }
        crlfPosition = this->_buffer.find(CRLF);
        hasCRLF = crlfPosition != this->_buffer.npos;
    }

    // handle HEAD method the same as GET read RFC
    // https://www.rfc-editor.org/rfc/rfc7231#section-4
    if (this->_status == READING_BODY_STATUS && this->_method.compare(HTTP_GET) == 0)
    {
        this->_status = REQUEST_READY_STATUS;
        return;
    }

    if (this->_status == READING_BODY_STATUS && this->_headers.count("content-length") == 0)
    {
        throw HttpBadRequestException();
    }
}

off_t HttpRequestHandler::serveStatic(const std::string &path, int httpStatus, const std::string &statusMessage)
{
    try
    {
        std::stringstream headers;
        const FileStat &stat = FileStat::open(path);

        headers << "HTTP/1.1 " << httpStatus << " " << statusMessage << CRLF;
        headers << "Content-Length: " << stat.getSize() << CRLF;
        // headers << "Content-Type: " << this->getFileContentType(path) << CRLF;
        headers << CRLF;

        const std::string &headersString = headers.str();
        (void)::send(this->_connectionRef, headersString.c_str(), headersString.length(), 0);

        this->sendFile(path);

        return stat.getSize();
    }
    catch (const std::exception &e)
    {
        throw HttpNotFoundException();
    }
}

off_t HttpRequestHandler::serveIndexFile(const std::string &path, std::vector<std::string> indexs, bool autoIndex)
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
                return this->directoryListing(path);
            else
                throw HttpForbiddenException();
        }
        return this->serveStatic(indexPath, HTTP_OK, HTTP_OK_MESSAGE);
    }
    catch (const std::exception &e)
    {
        throw HttpForbiddenException();
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
    // headers << "Content-Type: " << this->getFileContentType(path) << CRLF;
    headers << CRLF;

    const std::string &response = headers.str() + stringBody;
    (void)::send(this->_connectionRef, response.c_str(), response.length(), 0);
    return stringBody.length();
}

void HttpRequestHandler::sendFile(const std::string &path) const
{
    std::ifstream file;
    char readBuffer[32];

    file.open(path.c_str());

    while (true)
    {
        std::istream &i = file.read(readBuffer, 32);
        if (i.gcount() == 0)
            break;
        (void)::send(this->_connectionRef, readBuffer, i.gcount(), 0);
    }
}

HttpRequestHandler::~HttpRequestHandler()
{
}

/**
 * exceptions
 */

/**
 * HttpRequestException
 */

int HttpRequestHandler::AHttpRequestException::getHttpStatus(void) const throw()
{
    return this->_httpStatus;
}

const char *HttpRequestHandler::AHttpRequestException::what() const throw()
{
    return this->_message.c_str();
}

HttpRequestHandler::AHttpRequestException::~AHttpRequestException() throw()
{
}

/**
 * HttpBadRequestException
 */
HttpRequestHandler::HttpBadRequestException::HttpBadRequestException(void) throw()
{
    this->_httpStatus = HTTP_BAD_REQUEST;
    this->_message = HTTP_BAD_REQUEST_MESSAGE;
}

HttpRequestHandler::HttpBadRequestException::~HttpBadRequestException() throw()
{
}

/**
 * HttpNotFoundException
 */
HttpRequestHandler::HttpNotFoundException::HttpNotFoundException(void) throw()
{
    this->_httpStatus = HTTP_NOT_FOUND;
    this->_message = HTTP_NOT_FOUND_MESSAGE;
}

HttpRequestHandler::HttpNotFoundException::~HttpNotFoundException() throw()
{
}

/**
 * HttpRequestTimeoutException
 */
HttpRequestHandler::HttpRequestTimeoutException::HttpRequestTimeoutException(void) throw()
{
    this->_httpStatus = HTTP_REQUEST_TIMEOUT;
    this->_message = HTTP_REQUEST_TIMEOUT_MESSAGE;
}

HttpRequestHandler::HttpRequestTimeoutException::~HttpRequestTimeoutException() throw()
{
}

/**
 * HttpForbidden
 */

HttpRequestHandler::HttpForbiddenException::HttpForbiddenException(void) throw()
{
    this->_httpStatus = HTTP_FORBIDDEN;
    this->_message = HTTP_FORBIDDEN_MESSAGE;
}

HttpRequestHandler::HttpForbiddenException::~HttpForbiddenException() throw()
{
}
