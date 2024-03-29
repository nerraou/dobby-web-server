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

class ServerSocket
{
private:
    int _port;
    int _socketRef;

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
    int accept(sockaddr_in &remoteSin, socklen_t &addressSize);

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
