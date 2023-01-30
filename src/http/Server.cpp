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

bool Server::resumeWriting(HttpRequestHandler &requestHandler)
{
    try
    {
        if (requestHandler.isWritingRequestBody())
        {
            requestHandler.resumeWritingRequestBody();
            return true;
        }
        else if (requestHandler.isWritingResponseBody())
        {
            requestHandler.resumeWritingResponseBody();
            return true;
        }
        return false;
    }
    catch (...)
    {
        throw HttpInternalServerErrorException();
    }
}

void Server::setEnvVars(void)
{
    ::setenv("SERVER_PORT", lib::toString(this->_config.getPort()).c_str(), 1);
    ::setenv("DOCUMENT_ROOT", this->_config.getRoot().c_str(), 1);
    if (this->_config.getServerNamesCount() > 0)
        ::setenv("SERVER_NAME", this->_config.getServerName(0).c_str(), 1);
}

bool Server::handleCGI(HttpRequestHandler &requestHandler, const std::string &path)
{
    if (!this->_config.getPHPCGIPath().empty() && lib::endsWith(path, ".php"))
    {
        if (!lib::isFileExist(path))
            throw HttpNotFoundException();

        requestHandler.setResponseHttpStatus(HTTP_OK);

        if (requestHandler.getHttpParser().isRequestMethodHasBody())
            requestHandler.setIsWritingRequestBodyStatus();
        else
            requestHandler.setIsDoneStatus();

        this->setEnvVars();
        requestHandler.runCGI(path, this->_config.getPHPCGIPath());
        return true;
    }
    return false;
}

void Server::start(HttpRequestHandler &requestHandler)
{
    if (this->resumeWriting(requestHandler))
        return;

    try
    {
        const std::string &path = this->_config.getRoot() + requestHandler.getHttpParser().getRequestTarget().path;
        bool hasTrainlingSlash;

        if (this->handleCGI(requestHandler, path))
            return;

        hasTrainlingSlash = path[path.length() - 1] == '/';

        requestHandler.setResponseHttpStatus(HTTP_OK);
        requestHandler.setIsWritingResponseBodyStatus();

        if (hasTrainlingSlash)
            requestHandler.serveIndexFile(path, this->_config.getIndexes(), this->_config.getAutoIndex());
        else
            requestHandler.serveStatic(path, HTTP_OK, HTTP_OK_MESSAGE);
    }
    catch (const AHttpRequestException &e)
    {
        requestHandler.setIsWritingResponseBodyStatus();
        requestHandler.setResponseHttpStatus(e.getHttpStatus());

        const std::string &path = this->_config.getRoot() + "/" + lib::toString(e.getHttpStatus()) + ".html";
        requestHandler.serveStatic(path, e.getHttpStatus(), e.what());
    }
}
