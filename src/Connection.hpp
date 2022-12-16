#pragma once

#include <string>
#include <poll.h>
#include <unistd.h>

class Connection
{
public:
    typedef struct pollfd PollFd;

private:
    int _socketRef;

public:
    Connection();
    Connection(int socketRef);
    ~Connection();

    ssize_t send(const char *buffer, size_t size);
    ssize_t send(const char *buffer);
    ssize_t send(const std::string &buffer);
    ssize_t receive(char *buffer, size_t size);

    void setSocketRef(int socketRef);
    int getSocketRef(void) const;
    void close(void);
    PollFd createPollData(void);
};
