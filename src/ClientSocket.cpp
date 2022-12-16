#include "ClientSocket.hpp"

ClientSocket::ClientSocket()
{
    this->setSocketRef(-1);
}

ClientSocket::~ClientSocket()
{
    // close connection
}

void ClientSocket::setSocketRef(int socketRef)
{
    this->_socketRef = socketRef;
}

int ClientSocket::getSocketRef(void) const
{
    return (this->_socketRef);
}
