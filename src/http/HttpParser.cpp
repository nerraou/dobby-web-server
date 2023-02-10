#include "HttpParser.hpp"

HttpParser::HttpParser()
{
    this->setReadingRequestLineStatus();
    this->_receivedBodySize = 0;
    this->_contentLength = 0;
    this->_chunkSize = -1;
    this->_isLastChunkSize = false;
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
    {
        if (lib::parseUnsignedLong(this->_headers.at(fieldName).c_str(), this->_contentLength) == false)
            throw HttpBadRequestException();
    }
    else if (fieldName.compare("transfer-encoding") == 0 && fieldValue.compare("chunked") != 0)
        throw HttpBadRequestException();
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

bool HttpParser::isReadingChunkedRequestBody(void) const
{
    return this->_status == ReadingChunkedRequestBody;
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

void HttpParser::setReadingChunkedRequestBodyStatus(void)
{
    this->_status = ReadingChunkedRequestBody;
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

void HttpParser::unchunkBody(void)
{
    ArrayBuffer::iterator begin;
    ArrayBuffer::iterator crlfPosition;
    long availableChunkSize;

    while (true)
    {
        begin = this->_buffer.begin();

        if (this->_chunkSize == 0)
        {
            if (this->_buffer.size() < 2)
                break;
            if (*begin != '\r' || *(begin + 1) != '\n')
                throw HttpBadRequestException();

            this->_buffer.erase(begin, begin + CRLF_LEN);

            this->_chunkSize = -1;
        }
        else if (this->_isLastChunkSize)
        {
            if (!this->_buffer.empty())
                throw HttpBadRequestException();
            this->setRequestReadyStatus();
            break;
        }
        else if (this->_chunkSize == -1)
        {
            crlfPosition = this->findCRLFPosition(begin);

            if (crlfPosition == this->_buffer.end())
                break;
            if (lib::parseLong(std::string(begin, crlfPosition), this->_chunkSize, 16) == false)
                throw HttpBadRequestException();
            this->_buffer.erase(begin, crlfPosition + CRLF_LEN);

            this->_isLastChunkSize = this->_chunkSize == 0;
        }
        else
        {
            if (this->_buffer.empty())
                break;
            availableChunkSize = this->_buffer.size();
            if (availableChunkSize > this->_chunkSize)
                availableChunkSize = this->_chunkSize;

            this->_body.insert(this->_body.end(), begin, begin + availableChunkSize);
            this->_buffer.erase(begin, begin + availableChunkSize);

            this->_chunkSize -= availableChunkSize;
        }
    }
}

void HttpParser::process(void)
{
    ArrayBuffer::iterator begin;
    ArrayBuffer::iterator crlfPosition;
    ArrayBuffer::iterator lastCRLFPosition;

    if (this->isRequestReady())
        return;
    if (this->isReadingChunkedRequestBody())
        return this->unchunkBody();
    if (this->isReadingRequestBody())
    {
        this->_body.insert(this->_body.end(), this->_buffer.begin(), this->_buffer.end());
        this->_buffer.clear();

        if (this->getReceivedBodySize() == this->getContentLength())
            this->setRequestReadyStatus();
        return;
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
            this->_receivedBodySize += this->_body.size();
            if (this->hasHeader("transfer-encoding"))
                this->setReadingChunkedRequestBodyStatus();
            else
                this->setReadingRequestBodyStatus();

            lastCRLFPosition = crlfPosition + CRLF_LEN;

            if (this->isReadingChunkedRequestBody())
            {
                this->_buffer.erase(this->_buffer.begin(), lastCRLFPosition);
                this->unchunkBody();
                return;
            }
            else
            {
                this->_body.insert(this->_body.end(), crlfPosition + CRLF_LEN, this->_buffer.end());
                this->_buffer.clear();
                break;
            }
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
        if (this->getReceivedBodySize() == this->getContentLength())
            this->setRequestReadyStatus();
    }
}

HttpParser::~HttpParser()
{
}
