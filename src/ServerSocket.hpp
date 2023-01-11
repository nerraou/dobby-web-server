#pragma once

#include <stdexcept>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

#include "Connection.hpp"

class ServerSocket
{
private:
    std::string _remoteAddress;
    int _port;
    int _socketRef;
    struct sockaddr_in _address;

    void setRemoteAddress(void);

public:
    ServerSocket();
    ~ServerSocket();

    /**
     * create and set the socket handle(_socketRef)
     * throw SocketCreateException if failed
     */
    void create(void);
    void bind(short port);
    void listen(int backlog);
    int accept(void);
    const std::string &getRemoteAddress(void) const;

    class ServerSocketException : public std::exception
    {
    protected:
        std::string _message;

    public:
        virtual ~ServerSocketException() throw();
    };

    class ServerSocketCreateException : public ServerSocketException
    {

    public:
        ServerSocketCreateException(const std::string &message);
        virtual const char *what() const throw();
        virtual ~ServerSocketCreateException() throw();
    };

    class ServerSocketBindException : public ServerSocketException
    {
    public:
        ServerSocketBindException(const std::string &message);
        virtual const char *what() const throw();
        virtual ~ServerSocketBindException() throw();
    };

    class ServerSocketListenException : public ServerSocketException
    {
    public:
        ServerSocketListenException(const std::string &message);
        virtual const char *what() const throw();
        virtual ~ServerSocketListenException() throw();
    };
};
