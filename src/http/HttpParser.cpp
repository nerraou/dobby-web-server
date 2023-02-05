#include "HttpParser.hpp"

HttpParser::HttpParser()
{
    this->setReadingRequestLineStatus();
    this->_receivedBodySize = 0;
}

void HttpParser::append(const char *buffer, int size)
{
    this->_buffer.insert(this->_buffer.end(), buffer, buffer + size);
}

void HttpParser::parseRequestLine(const ArrayBuffer::const_iterator &beginIt, const ArrayBuffer::const_iterator &endIt)
{
    ArrayBuffer::const_iterator start;
    ArrayBuffer::const_iterator end;

    end = std::find(beginIt, endIt, ' ');
    if (end == endIt)
        throw HttpBadRequestException();
    this->setMethod(std::string(beginIt, end));

    start = lib::findIfNot(end, endIt, ::isblank);

    if (start == endIt)
        throw HttpBadRequestException();

    end = std::find(start, endIt, ' ');

    try
    {
        this->_requestTarget = Url::parse(std::string(start, end));
    }
    catch (const Url::ParseUrlException &e)
    {
        throw HttpBadRequestException();
    }

    start = lib::findIfNot(end, endIt, ::isblank);

    if (start == endIt)
        throw HttpBadRequestException();

    end = std::find(start, endIt, ' ');

    this->setHttpVersion(std::string(start, end));
}

void HttpParser::parseRequestHeader(const ArrayBuffer::const_iterator &beginIt, const ArrayBuffer::const_iterator &endIt)
{
    ArrayBuffer::const_iterator start;
    ArrayBuffer::const_iterator end;
    std::string fieldName;
    std::string fieldValue;

    end = std::find(beginIt, endIt, ':');
    if (end == endIt)
        throw HttpBadRequestException();

    fieldName = std::string(beginIt, end);
    if (!this->isHeaderFieldName(fieldName))
        throw HttpBadRequestException();

    lib::transform(fieldName.begin(), fieldName.end(), std::tolower);
    fieldValue = lib::trim(std::string(end + 1, endIt));

    if (fieldValue.empty())
        throw HttpBadRequestException();

    this->_headers.insert(std::make_pair(fieldName, fieldValue));

    if (fieldName.compare("content-length") == 0)
        this->_contentLength = std::atol(this->_headers.at(fieldName).c_str());
}

void HttpParser::setMethod(const std::string &method)
{
    const std::string methods[] = {
        "GET",
        "POST",
        "PATCH",
        "PUT",
        "DELETE",
    };
    const int methodsCount = 5;

    for (int i = 0; i < methodsCount; i++)
    {
        if (method.compare(methods[i]) == 0)
        {
            this->_method = method;
            return;
        }
    }
    throw HttpBadRequestException();
}

void HttpParser::setHttpVersion(const std::string &httpVersion)
{
    if (httpVersion.compare("HTTP/1.1") != 0)
        throw HttpBadRequestException();

    this->_httpVersion = httpVersion;
}

bool HttpParser::isHeaderFieldName(const std::string &name)
{
    const char *set = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";

    if (name.empty())
        return (false);
    return name.find_first_not_of(set) == name.npos;
}

std::size_t HttpParser::getContentLength() const
{
    return this->_contentLength;
}

const std::string &HttpParser::getMethod(void) const
{
    return this->_method;
}

const std::string &HttpParser::getHttpVersion(void) const
{
    return this->_httpVersion;
}

const Url &HttpParser::getRequestTarget(void) const
{
    return this->_requestTarget;
}

const StringStringMap &HttpParser::getHeaders(void) const
{
    return this->_headers;
}

const std::string &HttpParser::getHeader(const std::string headerName) const
{
    return this->_headers.at(headerName);
}

bool HttpParser::hasHeader(const std::string headerName) const
{
    return this->_headers.count(headerName) != 0;
}

const ArrayBuffer &HttpParser::getBody(void) const
{
    return this->_body;
}

bool HttpParser::isRequestMethodHasBody(void) const
{
    return this->_method.compare("GET") != 0;
}

bool HttpParser::isReadingRequestBody(void) const
{
    return this->_status == ReadingRequestBody;
}

bool HttpParser::isReadingRequestLine(void) const
{
    return this->_status == ReadingRequestLine;
}

bool HttpParser::isReadingRequestHeaders(void) const
{
    return this->_status == ReadingRequestHeaders;
}

bool HttpParser::isRequestReady(void) const
{
    return this->_status == RequestReady;
}

void HttpParser::setReadingRequestLineStatus(void)
{
    this->_status = ReadingRequestLine;
}

void HttpParser::setReadingRequestBodyStatus(void)
{
    this->_status = ReadingRequestBody;
}

void HttpParser::setReadingRequestHeadersStatus(void)
{
    this->_status = ReadingRequestHeaders;
}

void HttpParser::setRequestReadyStatus(void)
{
    this->_status = RequestReady;
}

ArrayBuffer::iterator HttpParser::findCRLFPosition(ArrayBuffer::iterator &begin)
{
    ArrayBuffer::iterator crlfPosition;
    ArrayBuffer::iterator end;

    end = this->_buffer.end();

    crlfPosition = std::find(begin, end, '\r');
    if (crlfPosition == end || crlfPosition + 1 == end)
        return (end);
    if (*(crlfPosition + 1) == '\n')
        return (crlfPosition);
    return (end);
}

std::size_t HttpParser::getReceivedBodySize(void) const
{
    return this->_receivedBodySize;
}
void HttpParser::clearBody(void)
{
    this->_body.clear();
}

void HttpParser::process(void)
{
    ArrayBuffer::iterator begin;
    ArrayBuffer::iterator crlfPosition;
    ArrayBuffer::iterator lastCRLFPosition;

    if (this->isRequestReady())
        return;
    if (this->isReadingRequestBody())
    {
        this->_body.insert(this->_body.end(), this->_buffer.begin(), this->_buffer.end());
        this->_buffer.clear();
        if (this->getReceivedBodySize() == this->getContentLength())
            this->setRequestReadyStatus();
    }

    begin = this->_buffer.begin();
    crlfPosition = this->findCRLFPosition(begin);
    lastCRLFPosition = crlfPosition;

    if (crlfPosition == this->_buffer.end())
        return;

    while (crlfPosition != this->_buffer.end())
    {
        if (this->isReadingRequestLine())
        {
            this->parseRequestLine(begin, crlfPosition);
            begin = crlfPosition + CRLF_LEN;
            this->setReadingRequestHeadersStatus();
        }
        else if (crlfPosition == begin)
        {
            this->_body.insert(this->_body.end(), crlfPosition + CRLF_LEN, this->_buffer.end());
            this->_receivedBodySize += this->_body.size();
            this->_buffer.clear();
            this->setReadingRequestBodyStatus();
            lastCRLFPosition = crlfPosition + CRLF_LEN;
            break;
        }
        else if (this->isReadingRequestHeaders())
        {
            this->parseRequestHeader(begin, crlfPosition);
            begin = crlfPosition + CRLF_LEN;
        }
        lastCRLFPosition = crlfPosition + CRLF_LEN;
        crlfPosition = this->findCRLFPosition(begin);
    }

    if (!this->_buffer.empty())
        this->_buffer.erase(this->_buffer.begin(), lastCRLFPosition);

    if (this->isReadingRequestBody() && !this->isRequestMethodHasBody())
        this->setRequestReadyStatus();
    else if (this->isReadingRequestBody())
    {
        if (!this->hasHeader("content-length"))
            throw HttpBadRequestException();

        if (this->getReceivedBodySize() == this->getContentLength())
            this->setRequestReadyStatus();
    }
}

HttpParser::~HttpParser()
{
}
