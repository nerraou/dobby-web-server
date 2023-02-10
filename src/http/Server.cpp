#include "Server.hpp"

Server::Server(const ConfigServer &config)
{
    this->_configServer = config;
}

const std::string &Server::getRoot() const
{
    return this->_configServer.getRoot();
}

std::string Server::getErrorPagePath(int status)
{
    return this->_config.getErrorPagePath(status);
}

Server::~Server()
{
}

int Server::findLocationPathMatch(const std::string &path) const
{
    return this->_configServer.findLocationPathMatch(path);
}

const ConfigLocation &Server::getConfigLocation(int index) const
{
    return this->_configServer.getConfigLocation(index);
}

bool Server::isServerNameExist(const std::string &host) const
{
    return this->_configServer.isServerNameExist(host);
}

bool Server::resumeWriting(HttpRequestHandler &requestHandler, std::size_t clientMaxBodySize)
{
    try
    {
        if (requestHandler.isWritingRequestBody())
        {
            requestHandler.resumeWritingRequestBody(clientMaxBodySize);
            return true;
        }
        else if (requestHandler.isWritingResponseBody())
        {
            requestHandler.resumeWritingResponseBody();
            return true;
        }
        return false;
    }
    catch (const AHttpRequestException &e)
    {
        throw e;
    }
    catch (...)
    {
        throw HttpInternalServerErrorException();
    }
}

void Server::setEnvVars(void)
{
    ::setenv("SERVER_PORT", lib::toString(this->_configServer.getPort()).c_str(), 1);
    ::setenv("DOCUMENT_ROOT", this->_configServer.getRoot().c_str(), 1);
    if (this->_configServer.getServerNamesCount() > 0)
        ::setenv("SERVER_NAME", this->_configServer.getServerName(0).c_str(), 1);
}

bool Server::handleCGI(HttpRequestHandler &requestHandler, const std::string &path)
{
    if (!this->_configServer.getPHPCGIPath().empty() && lib::endsWith(path, ".php"))
    {
        if (!lib::isFileExist(path))
            throw HttpNotFoundException();

        requestHandler.setResponseHttpStatus(HTTP_OK);

        if (requestHandler.getHttpParser().isRequestMethodHasBody())
            requestHandler.setIsWritingRequestBodyStatus();
        else
            requestHandler.setIsDoneStatus();

        this->setEnvVars();
        requestHandler.runCGI(path, this->_configServer.getPHPCGIPath());
        return true;
    }
    return false;
}

void Server::handleClientMaxBodySize(const HttpRequestHandler &requestHandler)
{
    const std::size_t contentLength = requestHandler.getHttpParser().getContentLength();
    const std::size_t clientMaxBodySize = this->_config.getClientMaxBodySize();

    if (contentLength > clientMaxBodySize || requestHandler.getHttpParser().getReceivedBodySize() > clientMaxBodySize)
        throw HttpPayloadTooLargeException();
}

void Server::executeMethods(HttpRequestHandler &requestHandler, const std::string &path)
{
    bool hasTrainlingSlash;
    hasTrainlingSlash = path[path.length() - 1] == '/';

    if (hasTrainlingSlash)
        // if index is .php it must be handled by CGI
        return requestHandler.serveIndexFile(path, this->_config.getIndexes(), this->_config.getAutoIndex());

    if (this->handleCGI(requestHandler, path))
        return;
    else
    {
        const std::string method = requestHandler.getHttpParser().getMethod();
        if (this->_config.hasMethod(method) == false)
            throw HttpMethodNotAllowedException();
        if (method == HTTP_GET)
            requestHandler.executeGet(path, HTTP_OK, HTTP_OK_MESSAGE);
        else if (method == HTTP_DELETE)
            requestHandler.executeDelete(path);
        else if (method == HTTP_PUT)
            requestHandler.executePut(path, this->_config.getClientMaxBodySize());
    }
}

void Server::initConfig()
{
    this->_config = this->_configServer;
}

void Server::start(HttpRequestHandler &requestHandler)
{
    try
    {
        int locationIndex;

        locationIndex = this->findLocationPathMatch(requestHandler.getHttpParser().getRequestTarget().path);
        if (locationIndex != -1)
        {
            this->_config = this->getConfigLocation(locationIndex);
            if (this->_config.hasRewrite())
                return requestHandler.rewrite(this->_config.getRewrite());
        }
        else
            this->_config = this->_configServer;

        if (this->resumeWriting(requestHandler, this->_config.getClientMaxBodySize()))
            return;

        this->handleClientMaxBodySize(requestHandler);

        const std::string &path = this->_config.getRoot() + requestHandler.getHttpParser().getRequestTarget().path;
        this->executeMethods(requestHandler, path);
    }
    catch (const AHttpRequestException &e)
    {
        const int status = e.getHttpStatus();

        requestHandler.executeGet(this->_config.getErrorPagePath(status), status, e.what());
    }
}
