#include "Server.hpp"

Server::Server(const ConfigServer &config)
{
    this->_config = config;
}

const std::string &Server::getRoot() const
{
    return this->_config.getRoot();
}

Server::~Server()
{
}

bool Server::isServerNameExist(const std::string &host) const
{
    return this->_config.isServerNameExist(host);
}

void Server::start(HttpRequestHandler &requestHandler, const std::string &remoteAddress)
{
    try
    {
        const std::string &path = this->_config.getRoot() + requestHandler.getRequestTarget().path;
        bool hasTrainlingSlash;

        hasTrainlingSlash = requestHandler.getRequestTarget().path[requestHandler.getRequestTarget().path.size() - 1] == '/';
        off_t responseContentLength;

        if (hasTrainlingSlash)
            responseContentLength = requestHandler.serveIndexFile(path, this->_config.getIndexes(), this->_config.getAutoIndex());
        else
            responseContentLength = requestHandler.serveStatic(path, HTTP_OK, HTTP_OK_MESSAGE);
        throw HttpRequestHandler::AHttpRequestException();
    }
    catch (const HttpRequestHandler::AHttpRequestException &e)
    {
        const std::string &path = this->_config.getRoot() + "/" + lib::toString(e.getHttpStatus()) + ".html";
        off_t responseContentLength = requestHandler.serveStatic(path, e.getHttpStatus(), e.what());
        requestHandler.logAccess(e.getHttpStatus(), responseContentLength, remoteAddress);
        throw HttpRequestHandler::AHttpRequestException();
    }
}
