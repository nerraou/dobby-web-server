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

    this->_socket.listen(100);
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
                return;
            }

            const HttpParser &httpParser = requestHandler->getHttpParser();

            serverIndex = this->getServerIndex(httpParser);
            this->_virtualServers[serverIndex]->initConfig();

            this->handleTimeout(nowTimestamp, requestHandler->getRequestLastRead(), requestHandler->getRequestTimeout());

            canWrite = requestHandler->getHttpParser().isRequestReady() ||
                       requestHandler->getHttpParser().isReadingRequestBody() ||
                       requestHandler->isWritingResponseBody();

            if (connection->revents & POLLIN)
                requestHandler->read();
            if (canWrite && connection->revents & POLLOUT)
                this->_virtualServers.at(serverIndex)->start(*requestHandler);
        }
    }
    catch (const AHttpRequestException &e)
    {
        const int status = e.getHttpStatus();
        const std::string &path = this->_virtualServers[serverIndex]->getErrorPagePath(status);
        requestHandler->setIsWritingResponseBodyStatus();
        requestHandler->setResponseHttpStatus(status);
        requestHandler->serveStatic(path, status, e.what());
    }
}

ServerGroup::~ServerGroup()
{
}
