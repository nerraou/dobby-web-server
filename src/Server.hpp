#pragma once

#include <map>
#include <poll.h>
#include "ServerSocket.hpp"
#include "Connection.hpp"

class Server
{
private:
    typedef std::map<int, Connection *> ConnectionsMap;
    ServerSocket _socket;
    ConnectionsMap _connections;

public:
    Server(void);
    ~Server();

    void start(int port);
    struct pollfd *toPollConnections(void);
};
