#include "ServerSocket.hpp"

ServerSocket::ServerSocket(void)
{
    this->_socketRef = -1;
    ::memset(&this->_address, 0, sizeof(struct sockaddr));
}

ServerSocket::~ServerSocket()
{
    if (this->_socketRef >= 0)
        ::close(this->_socketRef);
}

void ServerSocket::create(void)
{
    int socketFlags;

    if (this->_socketRef >= 0)
        throw ServerSocket::ServerSocketCreateException("socket already initialized");

    this->_socketRef = ::socket(AF_INET, SOCK_STREAM, 0);

    if (this->_socketRef < 0)
        throw ServerSocket::ServerSocketCreateException(strerror(errno));

    socketFlags = fcntl(this->_socketRef, F_GETFL, 0);

    if (fcntl(this->_socketRef, F_SETFL, socketFlags | O_NONBLOCK) < 0)
        throw ServerSocket::ServerSocketCreateException(strerror(errno));
}

void ServerSocket::bind(short port)
{
    socklen_t addressSize = sizeof(struct sockaddr_in);

    this->_address.sin_family = AF_INET;
    this->_address.sin_addr.s_addr = INADDR_ANY;
    this->_address.sin_port = htons(port);

    if (::bind(this->_socketRef, (struct sockaddr *)&this->_address, addressSize) < 0)
        throw ServerSocket::ServerSocketBindException(strerror(errno));
}

void ServerSocket::listen(int backlog)
{
    if (::listen(this->_socketRef, backlog) < 0)
        throw ServerSocket::ServerSocketListenException(strerror(errno));
}

bool ServerSocket::accept(ClientSocket &client)
{
    socklen_t addressSize = sizeof(struct sockaddr_in);
    int acceptedSocketRef;

    acceptedSocketRef = ::accept(this->_socketRef, (struct sockaddr *)&this->_address, &addressSize);

    if (acceptedSocketRef >= 0)
    {
        client.setSocketRef(acceptedSocketRef);
        return (true);
    }

    return (false);
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

// /**
//  * SocketAcceptException
//  */
// ServerSocket::ServerSocketAcceptException::ServerSocketAcceptException(const std::string &message)
// {
//     this->_message = message;
// }

// const char *ServerSocket::ServerSocketAcceptException::what(void) const throw()
// {
//     return this->_message.c_str();
// }

// ServerSocket::ServerSocketAcceptException::~ServerSocketAcceptException() throw()
// {
// }
