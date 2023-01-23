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
        throw HttpRequestTimeoutException();
}

void ServerGroup::start(std::vector<PollFd> &connections)
{
    PollFd *connection;
    HttpRequestHandler *requestHandler = NULL;
    time_t nowTimestamp = ::time(NULL);
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
            requestHandler = &this->_requests.at(connectionFd);
            const HttpParser &httpParser = requestHandler->getHttpParser();

            this->handleTimeout(nowTimestamp, requestHandler->getRequestLastRead(), requestHandler->getRequestTimeout());
            if (connection->revents & POLLOUT && requestHandler->getHttpParser().isRequestReady())
            {
                serverIndex = 0;
                if (httpParser.hasHeader("host"))
                    serverIndex = this->hasServerHost(httpParser.getHeader("host"));

                if (serverIndex == -1)
                    serverIndex = 0;

                this->_virtualServers.at(serverIndex)->start(*requestHandler, this->_socket.getRemoteAddress());
            }
            else if (connection->revents & POLLIN)
                requestHandler->read();
        }
    }
    catch (const HttpRequestTimeoutException &e)
    {
        const std::string &path = this->_virtualServers[serverIndex]->getRoot() + "/" + lib::toString(e.getHttpStatus()) + ".html";

        off_t responseContentLength = requestHandler->serveStatic(path, e.getHttpStatus(), e.what());

        requestHandler->logAccess(e.getHttpStatus(), responseContentLength, this->_socket.getRemoteAddress());
        this->closeConnection(connection->fd);
    }
    catch (const AHttpRequestException &e)
    {
        this->closeConnection(connection->fd);
    }
}

ServerGroup::~ServerGroup()
{
}
