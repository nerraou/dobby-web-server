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

    this->setRequestTarget(line.substr(start, end - start));

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
}

int HttpRequestHandler::getConnectionRef(void) const
{
    return this->_connectionRef;
}

bool HttpRequestHandler::isRequestReady(void) const
{
    return this->_status == REQUEST_READY_STATUS;
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
HttpRequestHandler::HttpBadRequestException::HttpBadRequestException(void)
{
    this->_httpStatus = 400;
    this->_message = "Bad Request";
}

HttpRequestHandler::HttpBadRequestException::~HttpBadRequestException() throw()
{
}
