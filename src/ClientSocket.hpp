#pragma once

class ClientSocket
{
private:
    int _socketRef;

public:
    ClientSocket();
    ~ClientSocket();

    // read();
    void setSocketRef(int socketRef);
    int getSocketRef(void) const;
};
