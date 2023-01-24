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

HttpRequestHandler::HttpRequestHandler(int connectionRef)
{
    this->_connectionRef = connectionRef;
    this->_requestReadTimeout = 5;
    this->_requestLastRead = ::time(NULL);
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

void HttpRequestHandler::read(void)
{
    ssize_t receivedBytes;
    ArrayBuffer buffer;

    if (this->_httpParser.isRequestReady())
        return;

    this->_requestLastRead = ::time(NULL);

    buffer.reserve(1024);

    receivedBytes = ::recv(this->_connectionRef, &buffer[0], buffer.capacity(), 0);

    if (receivedBytes <= 0)
        return;

    this->_httpParser.append(buffer.begin(), buffer.begin() + receivedBytes);
    this->_httpParser.process();
}

off_t HttpRequestHandler::serveStatic(const std::string &path, int httpStatus, const std::string &statusMessage)
{
    try
    {
        std::stringstream headers;
        const FileStat &stat = FileStat::open(path);
        std::string contentType;

        contentType = this->getFileContentType(path);
        headers << "HTTP/1.1 " << httpStatus << " " << statusMessage << CRLF;
        headers << "Content-Length: " << stat.getSize() << CRLF;
        if (contentType.empty() == false)
            headers << "Content-Type: " << contentType << CRLF;
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

void HttpRequestHandler::logAccess(int httpStatus, std::size_t contentLength, const std::string &remoteAddress) const
{
    char dateString[27];

    const time_t nowTimestamp = ::time(NULL);

    lib::formatTime(dateString, 27, "%d/%b/%Y:%X %z", nowTimestamp);

    std::cout << remoteAddress << " - ["
              << dateString
              << "] \""
              << this->_httpParser.getMethod()
              << " "
              << this->_httpParser.getRequestTarget().origin
              << "\" "
              << httpStatus
              << " "
              << contentLength << std::endl;
}

HttpRequestHandler::~HttpRequestHandler()
{
}
