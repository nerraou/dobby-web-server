#include "ServerGroup.hpp"

bool ServerGroup::hasConnection(int fd)
{
    if (this->_connections.find(fd) != this->_connections.end())
        return true;
    return false;
}

int ServerGroup::hasServerHost(const std::string &host) const
{
    for (size_t i = 0; i < this->_virtualServers.size(); i++)
    {
        if (this->_virtualServers[i]->isServerNameExist(host))
            return i;
    }
    return -1;
}

ServerGroup::ServerGroup()
{
}

ServerGroup::ServerGroup(int serverGroupPort)
{
    this->_socket.create();
    this->_socket.bind(serverGroupPort);
    this->_socket.listen(SOMAXCONN);
}

void ServerGroup::addVirtualServer(const ConfigServer &configServer)
{
    Server *virtualServer;

    virtualServer = new Server(configServer);
    this->_virtualServers.push_back(virtualServer);
}

int ServerGroup::acceptConnection(void)
{
    int acceptedConnection;
    std::string remoteAddress;
    sockaddr_in remoteSin;
    socklen_t addressSize = sizeof(sockaddr_in);

    acceptedConnection = this->_socket.accept(remoteSin, addressSize);
    if (acceptedConnection != -1)
    {
        this->_connections.insert(acceptedConnection);
        this->_requests.insert(std::make_pair(acceptedConnection, new HttpRequestHandler(acceptedConnection, remoteSin)));
    }
    return (acceptedConnection);
}

void ServerGroup::closeConnection(int &fd)
{
    ::close(fd);
    this->_connections.erase(fd);
    delete this->_requests.at(fd);
    this->_requests.erase(fd);
    fd = -1;
}

int ServerGroup::getServerIndex(const HttpParser &httpParser) const
{
    int serverIndex;

    serverIndex = 0;
    if (httpParser.hasHeader("host"))
        serverIndex = this->hasServerHost(httpParser.getHeader("host"));
    if (serverIndex == -1)
        serverIndex = 0;
    return serverIndex;
}

void ServerGroup::handleTimeout(int nowTimestamp, int requestLastRead, int requestTimeout)
{
    if (nowTimestamp - requestLastRead >= requestTimeout)
        throw HttpRequestTimeoutException();
}

void ServerGroup::start(std::vector<PollFd> &connections)
{
    PollFd *connection;
    HttpRequestHandler *requestHandler = NULL;
    time_t nowTimestamp = ::time(NULL);
    bool canWrite;
    int connectionFd;
    int serverIndex;

    serverIndex = 0;
    try
    {
        for (size_t i = 0; i < connections.size(); i++)
        {
            connection = &connections.at(i);
            connectionFd = connection->fd;
            if (this->hasConnection(connectionFd) == false)
                continue;

            requestHandler = this->_requests.at(connectionFd);

            if (requestHandler->isDone())
            {
                requestHandler->logAccess();
                this->closeConnection(connection->fd);
                continue;
            }

            const HttpParser &httpParser = requestHandler->getHttpParser();
            serverIndex = this->getServerIndex(httpParser);

            if (!requestHandler->isTimeout())
                this->handleTimeout(nowTimestamp, requestHandler->getRequestLastRead(), requestHandler->getRequestTimeout());

            if (connection->revents & POLLERR || connection->revents & POLLNVAL || connection->revents & POLLHUP)
            {
                requestHandler->setIsDoneStatus();
                continue;
            }

            if (connection->revents & POLLIN)
                requestHandler->read();

            if (serverIndex == 0)
                serverIndex = this->getServerIndex(httpParser);

            canWrite = requestHandler->getHttpParser().isRequestReady() ||
                       requestHandler->getHttpParser().isReadingRequestBody() ||
                       requestHandler->getHttpParser().isReadingChunkedRequestBody() ||
                       requestHandler->isWritingResponseBody();

            this->_virtualServers[serverIndex]->initConfig();

            if (canWrite && connection->revents & POLLOUT)
                this->_virtualServers.at(serverIndex)->start(*requestHandler);
        }
    }
    catch (const AHttpRequestException &e)
    {
        const int status = e.getHttpStatus();
        const Server *server = this->_virtualServers.at(serverIndex);
        const Config &config = server->getConfig();
        const std::string &path = server->getErrorPagePath(status);

        requestHandler->setResponseHttpStatus(status);

        if (connection->revents & POLLOUT)
        {
            if (config.hasCGI(path))
                requestHandler->handleCGI(path, config.getCGIPath(path));
            else
                requestHandler->executeGet(path, status, e.what());
        }
        else
        {
            requestHandler->setIsDoneStatus();
        }
    }
}

ServerGroup::~ServerGroup()
{
    for (size_t i = 0; i < this->_virtualServers.size(); i++)
    {
        delete this->_virtualServers[i];
    }

    for (size_t i = 0; i < this->_requests.size(); i++)
    {
        delete this->_requests[i];
    }
}
