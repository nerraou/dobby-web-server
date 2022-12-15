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

    class SocketException : public std::exception
    {
    protected:
        std::string _message;

    public:
        virtual ~SocketException() throw();
    };

    class SocketCreateException : public SocketException
    {

    public:
        SocketCreateException(const std::string &message);
        virtual const char *what() const throw();
        virtual ~SocketCreateException() throw();
    };

    class SocketBindException : public SocketException
    {
    public:
        SocketBindException(const std::string &message);
        virtual const char *what() const throw();
        virtual ~SocketBindException() throw();
    };

    class SocketListenException : public SocketException
    {
    public:
        SocketListenException(const std::string &message);
        virtual const char *what() const throw();
        virtual ~SocketListenException() throw();
    };

    class SocketAcceptException : public SocketException
    {
    public:
        SocketAcceptException(const std::string &message);
        virtual const char *what() const throw();
        virtual ~SocketAcceptException() throw();
    };
};
