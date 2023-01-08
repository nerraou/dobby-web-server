#include "Http.hpp"

Http::Http(const ConfigHttp &config)
{
    Server *virtualServer;
    ConfigServer configServer;

    this->_config = config;

    for (size_t i = 0; i < this->_config.getServersCount(); i++)
    {
        configServer = this->_config.getServerConfig(i);
        virtualServer = new Server(configServer);
        this->_virtualServers.push_back(virtualServer);
    }
}

inline bool Http::comparePollFdByFd(const PollFd &a, const PollFd &b)
{
    return a.fd > b.fd;
}

inline bool Http::isBadPollFd(const PollFd &a)
{
    return a.fd == -1;
}

inline void Http::removeBadConnections(void)
{
    std::vector<PollFd>::iterator last;

    std::sort(this->_connections.begin(), this->_connections.end(), Http::comparePollFdByFd);
    last = std::find_if(this->_connections.begin(), this->_connections.end(), Http::isBadPollFd);

    if (last != this->_connections.end())
        this->_connections.erase(last);
}

void Http::start(void)
{
    while (true)
    {
        for (size_t i = 0; i < this->_config.getServersCount(); i++)
        {
            PollFd connection;

            connection.fd = this->_virtualServers.at(i)->acceptConnection();
            if (connection.fd != -1)
            {
                connection.events = POLLIN | POLLOUT;
                this->_connections.push_back(connection);
            }
        }

        if (this->_connections.size() == 0)
            continue;

        int readyCount = ::poll(&this->_connections[0], this->_connections.size(), 10);
        (void)readyCount;

        this->removeBadConnections();

        for (size_t i = 0; i < this->_virtualServers.size(); i++)
            this->_virtualServers[i]->start(this->_connections);
    }
}

Http::~Http()
{
    for (size_t i = 0; i < this->_virtualServers.size(); i++)
    {
        delete this->_virtualServers[i];
    }
}
