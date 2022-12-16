#include "Server.hpp"
#include <iostream>

Server::Server(void)
{
}

Server::~Server()
{
}

void Server::start(int port)
{
    Connection *newConnection;
    Connection::PollFd *pollConnections;
    char buffer[4096];
    int readyConnectionsCount;

    (void)readyConnectionsCount;

    this->_socket.create();
    this->_socket.bind(port);

    const char *response_body =
        "HTTP/1.1 200 OK\n"
        "Server: Dobby/0.0.0 (MacOS)\n"
        "Content-Length: 48\n"
        "Connection: Closed\n\n"
        "<html><body><h1>Hello, World!</h1></body></html>";

    while (true)
    {
        this->_socket.listen(100);
        newConnection = this->_socket.accept();

        if (newConnection)
            this->_connections[newConnection->getSocketRef()] = newConnection;
        pollConnections = this->toPollConnections();
        if (pollConnections)
        {
            readyConnectionsCount = ::poll(pollConnections, this->_connections.size(), 10);
            for (size_t i = 0; i < this->_connections.size(); i++)
            {
                if (pollConnections[i].revents & POLLIN)
                {
                    ::recv(pollConnections[i].fd, buffer, 4096, 0);
                }
                else if (pollConnections[i].revents & POLLOUT)
                {
                    ::send(pollConnections[i].fd, response_body, ::strlen(response_body), 0);
                    ::close(pollConnections[i].fd);
                }
            }
        }
        delete[] pollConnections;
    }
}

Connection::PollFd *Server::toPollConnections(void)
{
    Connection::PollFd *pollConnections;
    int i;
    ConnectionsMap::const_iterator it;

    pollConnections = new struct pollfd[this->_connections.size()];

    if (!pollConnections)
        return (NULL);

    i = 0;
    it = this->_connections.begin();
    while (it != this->_connections.end())
    {
        pollConnections[i] = it->second->createPollData();

        ++i;
        ++it;
    }
    return (pollConnections);
}
