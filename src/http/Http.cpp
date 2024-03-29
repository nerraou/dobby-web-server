#include "Http.hpp"

Http::Http(const ConfigHttp &config)
{
    this->_config = config;
    this->createServerGroups();
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

    if (this->_connections.empty())
        return;

    std::sort(this->_connections.begin(), this->_connections.end(), Http::comparePollFdByFd);
    last = std::find_if(this->_connections.begin(), this->_connections.end(), Http::isBadPollFd);

    if (last != this->_connections.end())
        this->_connections.erase(last, this->_connections.end());
}

void Http::createServerGroups()
{
    ServerGroup *serverGroup;

    for (size_t i = 0; i < this->_config.getServersCount(); i++)
    {
        int port;

        port = this->_config.getServerConfig(i).getPort();
        if (this->_serverGroups.count(port) == 0)
        {
            serverGroup = new ServerGroup(port);
            serverGroup->addVirtualServer(this->_config.getServerConfig(i));
            this->_serverGroups.insert(std::pair<int, ServerGroup *>(port, serverGroup));
        }
        else
        {
            this->_serverGroups[port]->addVirtualServer(this->_config.getServerConfig(i));
        }
    }
}

void Http::start(void)
{
    std::size_t maxPollConnections;
    std::map<int, ServerGroup *>::const_iterator it;
    int readyCount;

    while (true)
    {
        it = this->_serverGroups.begin();
        while (it != this->_serverGroups.end())
        {
            PollFd connection;

            connection.fd = it->second->acceptConnection();

            if (connection.fd != -1)
            {
                connection.events = POLLIN | POLLOUT;
                this->_connections.push_back(connection);
            }
            ++it;
        }

        if (this->_connections.size() == 0)
            continue;

        this->removeBadConnections();

        maxPollConnections = this->_connections.size();
        if (maxPollConnections > LIMIT_NOFILE)
            maxPollConnections = LIMIT_NOFILE;

        readyCount = ::poll(&this->_connections[0], maxPollConnections, 10);

        if (readyCount == -1)
            continue;

        it = this->_serverGroups.begin();
        while (it != this->_serverGroups.end())
        {
            it->second->start(this->_connections);
            ++it;
        }
    }
}

Http::~Http()
{
    std::map<int, ServerGroup *>::iterator it = this->_serverGroups.begin();
    while (it != this->_serverGroups.end())
    {
        delete it->second;
        ++it;
    }
}
