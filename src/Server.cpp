#include "Server.hpp"

Server::Server(const ConfigServer &config)
{
    this->_config = config;
    this->_socket.create();
    this->_socket.bind(this->_config.getPort());
}

Server::~Server()
{
}

int Server::acceptConnection(void)
{
    int acceptedConnection;

    this->_socket.listen(100);
    acceptedConnection = this->_socket.accept();
    if (acceptedConnection != -1)
    {
        this->_connections.insert(acceptedConnection);
        this->_requests.insert(std::make_pair(acceptedConnection, HttpRequestHandler(acceptedConnection)));
    }
    return (acceptedConnection);
}

void Server::closeConnection(int &fd)
{
    ::close(fd);
    this->_connections.erase(fd);
    this->_requests.erase(fd);
    fd = -1;
}

void Server::start(std::vector<PollFd> &connections)
{
    PollFd *connection;
    bool hasConnection;
    HttpRequestHandler *requestHandler = NULL;
    time_t nowTimestamp = ::time(NULL);

    for (size_t i = 0; i < connections.size(); i++)
    {
        try
        {
            connection = &connections.at(i);
            hasConnection = this->_connections.find(connection->fd) != this->_connections.end();
            if (!hasConnection)
                continue;
            requestHandler = &this->_requests.at(connection->fd);

            if (requestHandler->getRequestLastRead() != -1)
            {
                if (nowTimestamp - requestHandler->getRequestLastRead() >= requestHandler->getRequestTimeout())
                    throw HttpRequestHandler::HttpRequestTimeoutException();
            }

            if (connection->revents & POLLOUT && requestHandler->isRequestReady())
            {
                if (requestHandler->getRequestTarget().path.empty())
                    throw HttpRequestHandler::HttpBadRequestException();

                const std::string &path = this->_config.getRoot() + requestHandler->getRequestTarget().path;
                bool hasTrainlingSlash;

                hasTrainlingSlash = requestHandler->getRequestTarget().path[requestHandler->getRequestTarget().path.size() - 1] == '/';
                off_t responseContentLength;

                if (hasTrainlingSlash)
                    responseContentLength = requestHandler->serveIndexFile(path, this->_config.getIndexes());
                else
                    responseContentLength = requestHandler->serveStatic(path, HTTP_OK, HTTP_OK_MESSAGE);

                this->logAccess(*requestHandler, HTTP_OK, responseContentLength);
                this->closeConnection(connection->fd);
            }
            else if (connection->revents & POLLIN)
                requestHandler->read();
        }
        catch (const HttpRequestHandler::AHttpRequestException &e)
        {
            const std::string &path = this->_config.getRoot() + "/" + lib::toString(e.getHttpStatus()) + ".html";

            off_t responseContentLength = requestHandler->serveStatic(path, e.getHttpStatus(), e.what());

            this->logAccess(*requestHandler, e.getHttpStatus(), responseContentLength);

            this->closeConnection(connection->fd);
        }
    }
}

void Server::logAccess(const HttpRequestHandler &request, int httpStatus, std::size_t contentLength) const
{
    char dateString[27];

    const time_t nowTimestamp = ::time(NULL);

    lib::formatTime(dateString, 27, "%d/%b/%Y:%X %z", nowTimestamp);

    std::cout << this->_socket.getRemoteAddress() << " - ["
              << dateString
              << "] \""
              << request.getMethod()
              << " "
              << request.getRequestTarget().origin
              << "\" "
              << httpStatus
              << " "
              << contentLength << std::endl;
}
