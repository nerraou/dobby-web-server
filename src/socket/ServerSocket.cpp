#include "ServerSocket.hpp"

ServerSocket::ServerSocket(void)
{
    this->_socketRef = -1;
}

ServerSocket::~ServerSocket()
{
    if (this->_socketRef >= 0)
        ::close(this->_socketRef);
}

void ServerSocket::create(void)
{
    int socketOption;

    if (this->_socketRef >= 0)
        throw ServerSocket::ServerSocketCreateException("socket already initialized");

    this->_socketRef = ::socket(AF_INET, SOCK_STREAM, 0);

    if (this->_socketRef < 0)
        throw ServerSocket::ServerSocketCreateException(strerror(errno));

    if (::fcntl(this->_socketRef, F_SETFL, O_NONBLOCK) < 0)
        throw ServerSocket::ServerSocketCreateException(strerror(errno));
    socketOption = 1;
    ::setsockopt(this->_socketRef, SOL_SOCKET, SO_REUSEADDR, &socketOption, sizeof(int));
}

void ServerSocket::bind(short port)
{
    socklen_t addressSize = sizeof(struct sockaddr_in);
    sockaddr_in addressSin;

    addressSin.sin_family = AF_INET;
    addressSin.sin_addr.s_addr = INADDR_ANY;
    addressSin.sin_port = htons(port);

    this->_port = port;

    if (::bind(this->_socketRef, (sockaddr *)&addressSin, addressSize) < 0)
        throw ServerSocket::ServerSocketBindException(strerror(errno));
}

void ServerSocket::listen(int backlog)
{
    if (::listen(this->_socketRef, backlog) < 0)
        throw ServerSocket::ServerSocketListenException(strerror(errno));
}

int ServerSocket::accept(sockaddr_in &remoteSin, socklen_t &addressSize)
{
    int acceptedSocketRef;

    acceptedSocketRef = ::accept(this->_socketRef, (sockaddr *)&remoteSin, &addressSize);

    return (acceptedSocketRef);
}

/**
 * ServerSocketException
 */

ServerSocket::ServerSocketException::~ServerSocketException() throw()
{
}

/**
 * ServerSocketCreateException
 */
ServerSocket::ServerSocketCreateException::ServerSocketCreateException(const std::string &message)
{
    this->_message = message;
}

const char *ServerSocket::ServerSocketCreateException::what(void) const throw()
{
    return this->_message.c_str();
}

ServerSocket::ServerSocketCreateException::~ServerSocketCreateException() throw()
{
}

/**
 * ServerSocketBindException
 */
ServerSocket::ServerSocketBindException::ServerSocketBindException(const std::string &message)
{
    this->_message = message;
}

const char *ServerSocket::ServerSocketBindException::what(void) const throw()
{
    return this->_message.c_str();
}

ServerSocket::ServerSocketBindException::~ServerSocketBindException() throw()
{
}

/**
 * ServerSocketListenException
 */
ServerSocket::ServerSocketListenException::ServerSocketListenException(const std::string &message)
{
    this->_message = message;
}

const char *ServerSocket::ServerSocketListenException::what(void) const throw()
{
    return this->_message.c_str();
}

ServerSocket::ServerSocketListenException::~ServerSocketListenException() throw()
{
}
