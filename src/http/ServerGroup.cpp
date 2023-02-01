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
    this->_socket.listen(1000);
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
                std::cerr << "connection count: " << this->_connections.size() << std::endl;
                requestHandler->logAccess();
                this->closeConnection(connection->fd);
                return;
            }

            const HttpParser &httpParser = requestHandler->getHttpParser();

            this->handleTimeout(nowTimestamp, requestHandler->getRequestLastRead(), requestHandler->getRequestTimeout());
            canWrite = requestHandler->getHttpParser().isRequestReady() ||
                       requestHandler->getHttpParser().isReadingRequestBody() ||
                       requestHandler->isWritingResponseBody();
            std::cerr << "is ready: " << requestHandler->getHttpParser().isRequestReady() << std::endl;
            std::cerr << "has POLLOUT: " << (connection->revents & POLLOUT) << std::endl;

            if (connection->revents & POLLIN)
            {
                std::cerr << "reading..." << std::endl;
                requestHandler->read();
            }
            if (canWrite && connection->revents & POLLOUT)
            {
                std::cerr << "writing..." << std::endl;
                serverIndex = 0;
                if (httpParser.hasHeader("host"))
                    serverIndex = this->hasServerHost(httpParser.getHeader("host"));

                if (serverIndex == -1)
                    serverIndex = 0;

                this->_virtualServers.at(serverIndex)->start(*requestHandler);
            }
        }
    }
    catch (const AHttpRequestException &e)
    {
        const std::string &path = this->_virtualServers[serverIndex]->getRoot() + "/" + lib::toString(e.getHttpStatus()) + ".html";

        requestHandler->setIsWritingResponseBodyStatus();
        requestHandler->setResponseHttpStatus(e.getHttpStatus());
        requestHandler->serveStatic(path, e.getHttpStatus(), e.what());
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
