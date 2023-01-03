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

void Server::start(std::vector<PollFd> &connections)
{
    PollFd *connection;
    bool hasConnection;
    std::ifstream file;
    char readBuffer[32];
    const std::string filename("/home/youssef/Projects/personal/dobby/tmp/www/index-1.html");
    std::string headers = "HTTP/1.1 200 OK\r\n"
                          "Server: Dobby/0.0.0 (MacOS)\r\n"
                          "Content-Length: ";

    for (size_t i = 0; i < connections.size(); i++)
    {
        try
        {
            connection = &connections.at(i);
            hasConnection = this->_connections.find(connection->fd) != this->_connections.end();
            if (!hasConnection)
                continue;
            HttpRequestHandler &requestHandler = this->_requests.at(connection->fd);

            if (connection->revents & POLLIN)
            {
                requestHandler.read();
            }
            else if (connection->revents & POLLOUT && requestHandler.isRequestReady())
            {
                struct stat fileStat;
                (void)::stat(filename.c_str(), &fileStat);
                headers.append(lib::toString(fileStat.st_size));
                headers.append("\r\n\r\n");
                (void)::send(connection->fd, headers.c_str(), headers.length(), 0);

                file.open(filename.c_str());
                while (true)
                {
                    std::istream &i = file.read(readBuffer, 32);
                    if (i.gcount() == 0)
                        break;
                    (void)::send(connection->fd, readBuffer, i.gcount(), 0);
                }

                ::close(connection->fd);
                this->_connections.erase(connection->fd);
                this->_requests.erase(connection->fd);
                connection->fd = -1;
            }
        }
        catch (const HttpRequestHandler::HttpBadRequestException &e)
        {
            std::string body = "HTTP/1.1 400 Bad Request\r\n"
                               "Server: Dobby/0.0.0 (MacOS)\r\n"
                               "Content-Length: 11\r\n"
                               "\r\n"
                               "Bad Request";

            (void)::send(connection->fd, body.c_str(), body.length(), 0);

            ::close(connection->fd);
            this->_connections.erase(connection->fd);
            this->_requests.erase(connection->fd);
            connection->fd = -1;
        }
    }
}
