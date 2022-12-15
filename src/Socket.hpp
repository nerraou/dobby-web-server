#pragma once

#include <exception>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

class Socket
{
private:
    int _port;
    int _socketRef;
    struct sockaddr_in _address;

public:
    Socket();
    ~Socket();

    /**
     * create and set the socket handle(_socketRef)
     * throw SocketCreateException if failed
     */
    void create(void);
    void bind(short port);
    void listen(int backlog);
    void accept(Socket &socket);

    class SocketException : std::exception
    {
    };

    class SocketCreateException : SocketException
    {
    private:
        std::string _message;

    public:
        SocketCreateException(const std::string &message);
        virtual const char *what() const throw();
    };

    class SocketBindException : SocketException
    {
    private:
        std::string _message;

    public:
        SocketBindException(const std::string &message);
        virtual const char *what() const throw();
    };

    class SocketListenException : SocketException
    {
    private:
        std::string _message;

    public:
        SocketListenException(const std::string &message);
        virtual const char *what() const throw();
    };

    class SocketAcceptException : SocketException
    {
    private:
        std::string _message;

    public:
        SocketAcceptException(const std::string &message);
        virtual const char *what() const throw();
    };
};
