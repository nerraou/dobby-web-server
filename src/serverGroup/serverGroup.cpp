#include "ServerGroup.hpp"

bool ServerGroup::hasConnection(int fd)
{
    if (this->_connections.find(fd) != this->_connections.end())
        return true;
    return false;
}

ServerGroup::ServerGroup()
{
}

ServerGroup::ServerGroup(const ConfigServer &configServer)
{
    Server *virtualServer;
    this->_config = configServer;

    this->_socket.create();
    this->_socket.bind(this->_config.getPort());
    virtualServer = new Server(this->_config);
    this->_virtualServers.push_back(virtualServer);
}

void ServerGroup::addServerGroup(const ConfigServer &configServer)
{
    ServerGroup(configServer);
}

int ServerGroup::acceptConnection(void)
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

void ServerGroup::closeConnection(int &fd)
{
    ::close(fd);
    this->_connections.erase(fd);
    this->_requests.erase(fd);
    fd = -1;
}

void ServerGroup::handleTimeout(int nowTimestamp, int requestLastRead, int requestTimeout)
{
    if (nowTimestamp - requestLastRead >= requestTimeout)
        throw HttpRequestHandler::HttpRequestTimeoutException();
}

void ServerGroup::start(std::vector<PollFd> &connections)
{
    PollFd *connection;
    HttpRequestHandler *requestHandler = NULL;
    time_t nowTimestamp = ::time(NULL);
    int connectionFd;

    try
    {
        for (size_t i = 0; i < connections.size(); i++)
        {
            connection = &connections.at(i);
            connectionFd = connection->fd;
            if (this->hasConnection(connectionFd) == false)
                continue;
            requestHandler = &this->_requests.at(connectionFd);
            this->handleTimeout(nowTimestamp, requestHandler->getRequestLastRead(), requestHandler->getRequestTimeout());
            if (connection->revents & POLLOUT && requestHandler->isRequestReady())
            {
                
            }

        }
    }
    catch (const HttpRequestHandler::AHttpRequestException &e)
    {
        const std::string &path = this->_config.getRoot() + "/" + lib::toString(e.getHttpStatus()) + ".html";

        off_t responseContentLength = requestHandler->serveStatic(path, e.getHttpStatus(), e.what());

        requestHandler->logAccess(*requestHandler, e.getHttpStatus(), responseContentLength);

        this->closeConnection(connection->fd);
    }
}

ServerGroup::~ServerGroup()
{
}
