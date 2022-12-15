#include "Socket.hpp"

Socket::Socket(void)
{
    this->_socketRef = -1;
    ::memset(&this->_address, 0, sizeof(struct sockaddr));
}

Socket::~Socket()
{
    if (this->_socketRef >= 0)
        ::close(this->_socketRef);
}

void Socket::create(void)
{
    int socketFlags;

    if (this->_socketRef >= 0)
        throw Socket::SocketCreateException("socket already initialized");

    this->_socketRef = ::socket(AF_INET, SOCK_STREAM, 0);

    if (this->_socketRef < 0)
        throw Socket::SocketCreateException(strerror(errno));

    socketFlags = fcntl(this->_socketRef, F_GETFL, 0);

    if (fcntl(this->_socketRef, F_SETFL, socketFlags | O_NONBLOCK) < 0)
        throw Socket::SocketCreateException(strerror(errno));
}

void Socket::bind(short port)
{
    socklen_t addressSize = sizeof(struct sockaddr_in);

    this->_address.sin_family = AF_INET;
    this->_address.sin_addr.s_addr = INADDR_ANY;
    this->_address.sin_port = htons(port);

    if (::bind(this->_socketRef, (struct sockaddr *)&this->_address, addressSize) < 0)
        throw Socket::SocketBindException(strerror(errno));
}

void Socket::listen(int backlog)
{
    if (::listen(this->_socketRef, backlog) < 0)
        throw Socket::SocketListenException(strerror(errno));
}

void Socket::accept(Socket &socket)
{
    socklen_t addressSize = sizeof(struct sockaddr_in);
    int acceptedSocketRef;

    acceptedSocketRef = ::accept(this->_socketRef, (struct sockaddr *)&this->_address, &addressSize);

    if (acceptedSocketRef < 0)
        throw Socket::SocketAcceptException(strerror(errno));

    socket._socketRef = acceptedSocketRef;
}

/**
 * SocketException
 */

Socket::SocketException::~SocketException() throw()
{
}

/**
 * SocketCreateException
 */
Socket::SocketCreateException::SocketCreateException(const std::string &message)
{
    this->_message = message;
}

const char *Socket::SocketCreateException::what(void) const throw()
{
    return this->_message.c_str();
}

Socket::SocketCreateException::~SocketCreateException() throw()
{
}

/**
 * SocketBindException
 */
Socket::SocketBindException::SocketBindException(const std::string &message)
{
    this->_message = message;
}

const char *Socket::SocketBindException::what(void) const throw()
{
    return this->_message.c_str();
}

Socket::SocketBindException::~SocketBindException() throw()
{
}

/**
 * SocketListenException
 */
Socket::SocketListenException::SocketListenException(const std::string &message)
{
    this->_message = message;
}

const char *Socket::SocketListenException::what(void) const throw()
{
    return this->_message.c_str();
}

Socket::SocketListenException::~SocketListenException() throw()
{
}

/**
 * SocketAcceptException
 */
Socket::SocketAcceptException::SocketAcceptException(const std::string &message)
{
    this->_message = message;
}

const char *Socket::SocketAcceptException::what(void) const throw()
{
    return this->_message.c_str();
}

Socket::SocketAcceptException::~SocketAcceptException() throw()
{
}
