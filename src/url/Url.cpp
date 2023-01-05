
#include "Url.hpp"

void Url::parseFragment(std::string &urlString)
{
    const std::size_t cutPos = urlString.find("#");

    if (cutPos != std::string::npos)
    {
        this->fragment.append(urlString, cutPos);
        urlString.erase(cutPos);
    }
}

void Url::parseQueryString(std::string &urlString)
{
    const std::size_t cutPos = urlString.find("?");

    if (cutPos != std::string::npos)
    {
        this->queryString.append(urlString, cutPos + 1);
        urlString.erase(cutPos);
    }
}

void Url::parseScheme(std::string &urlString)
{
    const std::size_t cutPos = urlString.find("://");
    std::string scheme;

    if (cutPos == std::string::npos)
        return;

    scheme.append(urlString, 0, cutPos);

    if (scheme.compare("http") != 0 && scheme.compare("https") != 0)
        throw ParseUrlException("bad url scheme");

    this->scheme.append(scheme);
    urlString.erase(0, cutPos + 3);
}

void Url::parsePath(std::string &urlString)
{
    const std::size_t cutPos = urlString.find("/");

    if (cutPos != std::string::npos)
    {
        this->path.append(urlString, cutPos + 1);
        urlString.erase(cutPos);
    }
}

void Url::parsePort(std::string &urlString)
{
    const std::size_t cutPos = urlString.find(":");
    std::string port;

    if (cutPos != std::string::npos)
    {
        port.append(urlString, cutPos + 1);

        if (port.empty() || port.find_first_not_of("0123456789") != std::string::npos)
            throw ParseUrlException("bad port");

        this->port = port;
        urlString.erase(cutPos);
    }
}

void Url::parseHost(std::string &urlString)
{
    if ((!this->scheme.empty() || !this->port.empty()) && urlString.empty())
        throw ParseUrlException("bad hostname");

    if (urlString.find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-.") != std::string::npos)
        throw ParseUrlException("bad hostname");

    this->host = urlString;
}

Url Url::parse(const std::string &urlString)
{
    Url url;
    std::string urlStringCopy(urlString);

    url.parseFragment(urlStringCopy);

    url.parseQueryString(urlStringCopy);

    url.parseScheme(urlStringCopy);

    url.parsePath(urlStringCopy);

    url.parsePort(urlStringCopy);

    url.parseHost(urlStringCopy);

    return url;
}

Url::ParseUrlException::ParseUrlException(const std::string &messsage) throw()
{
    this->_message = messsage;
}

const char *Url::ParseUrlException::what() const throw()
{
    return this->_message.c_str();
}

Url::ParseUrlException::~ParseUrlException() throw()
{
}
