#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "typedefs.hpp"
#include "lib.hpp"
#include "Url.hpp"
#include "HttpException.hpp"

#define CRLF_LEN 2

class HttpParser
{
public:
    enum RequestStatus
    {
        ReadingRequestLine = 0,
        ReadingRequestHeaders = 1,
        ReadingRequestBody = 2,
        ReadingChunkedRequestBody = 3,
        RequestReady = 4
    };

private:
    ArrayBuffer _buffer;
    ArrayBuffer _body;
    StringStringMap _headers;
    std::string _method;
    std::string _httpVersion;
    Url _requestTarget;
    std::size_t _contentLength;
    std::size_t _receivedBodySize;
    long _chunkSize;
    bool _isLastChunkSize;
    RequestStatus _status;

    void
    setMethod(const std::string &method);
    void setHttpVersion(const std::string &httpVersion);
    bool isHeaderFieldName(const std::string &name);
    bool isReadingRequestLine(void) const;
    bool isReadingRequestHeaders(void) const;
    void setReadingRequestLineStatus(void);
    void setReadingRequestBodyStatus(void);
    void setReadingRequestHeadersStatus(void);
    void setRequestReadyStatus(void);
    void setReadingChunkedRequestBodyStatus(void);
    void unchunkBody(void);
    ArrayBuffer::iterator findCRLFPosition(ArrayBuffer::iterator &begin);

public:
    void parseRequestLine(const ArrayBuffer::const_iterator &beginIt, const ArrayBuffer::const_iterator &endIt);
    void parseRequestHeader(const ArrayBuffer::const_iterator &beginIt, const ArrayBuffer::const_iterator &endIt);

    HttpParser();
    void append(const char *buffer, int size);
    void process(void);
    const std::string &getMethod(void) const;
    const std::string &getHttpVersion(void) const;
    const Url &getRequestTarget(void) const;
    const StringStringMap &getHeaders(void) const;
    const std::string &getHeader(const std::string &headerName) const;
    bool hasHeader(const std::string &headerName) const;
    const ArrayBuffer &getBody(void) const;

    bool isRequestReady(void) const;
    bool isReadingRequestBody(void) const;
    bool isReadingChunkedRequestBody(void) const;
    bool isRequestMethodHasBody(void) const;
    std::size_t getContentLength() const;
    std::size_t getReceivedBodySize(void) const;
    void clearBody(void);

    ~HttpParser();

    friend class HttpRequestHandler;
};
