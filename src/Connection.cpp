#include "Connection.hpp"

Connection::Connection()
{
    this->setSocketRef(-1);
}

Connection::Connection(int socketRef)
{
    this->setSocketRef(socketRef);
}

Connection::~Connection()
{
    this->close();
}

void Connection::close(void)
{
    if (this->_socketRef != -1)
    {
        ::close(this->_socketRef);
        this->_socketRef = -1;
    }
}

void Connection::setSocketRef(int socketRef)
{
    this->_socketRef = socketRef;
}

int Connection::getSocketRef(void) const
{
    return (this->_socketRef);
}

Connection::PollFd Connection::createPollData(void)
{
    PollFd pollData;

    ::memset(&pollData, 0, sizeof(struct pollfd));

    pollData.fd = this->_socketRef;
    pollData.events = POLLIN | POLLOUT;

    return pollData;
}
