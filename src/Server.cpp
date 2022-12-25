#include "Server.hpp"
#include <iostream>

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
        this->_connections.insert(acceptedConnection);
    return (acceptedConnection);
}

void Server::start(std::vector<PollFd> &connections)
{
    PollFd *connection;
    bool hasConnection;
    ssize_t readedBytes;

    char buffer[4096];
    std::stringstream response;
    std::stringstream responseBody;

    response << "HTTP/1.1 200 OK\n"
                "Server: Dobby/0.0.0 (MacOS)\n"
                "Content-Length: ";
    responseBody << "<html><body><h1>Hello, World! from: ";

    for (size_t i = 0; i < connections.size(); i++)
    {
        connection = &connections.at(i);
        hasConnection = this->_connections.find(connection->fd) != this->_connections.end();
        if (!hasConnection)
            continue;
        if (connection->revents & POLLIN)
        {
            readedBytes = ::recv(connection->fd, buffer, 4096, 0);
            if (readedBytes > 0)
                ::write(1, buffer, readedBytes);
        }
        else if (connection->revents & POLLOUT)
        {
            responseBody << this->_config.getPort() << "</h1></body></html>";
            std::string str = responseBody.str();
            response << str.length() << "\n\n"
                     << str;

            std::string body = response.str();
            (void)::send(connection->fd, body.c_str(), body.length(), 0);
            ::close(connection->fd);
            this->_connections.erase(connection->fd);
            connection->fd = -1;
        }
    }
}
