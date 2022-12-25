#include "Http.hpp"

Http::Http(const ConfigHttp &config)
{
    this->_config = config;
}

void Http::start(void)
{
    Server *virtualServer;
    ConfigServer configServer;
    PollFd connection;

    for (size_t i = 0; i < this->_config.getServersCount(); i++)
    {
        configServer = this->_config.getServerConfig(i);
        virtualServer = new Server(configServer);
        this->_virtualServers.push_back(virtualServer);
    }

    std::cout << "servers count: " << this->_config.getServersCount() << std::endl;
    while (true)
    {
        // accept new connections
        for (size_t i = 0; i < this->_config.getServersCount(); i++)
        {
            connection.fd = this->_virtualServers.at(i)->acceptConnection();
            if (connection.fd != -1)
            {
                connection.events = POLLIN | POLLOUT;
                this->_connections.push_back(connection);
            }
        }
        if (this->_connections.size() == 0)
            continue;
        // run poll
        int readyCount = ::poll(&this->_connections[0], this->_connections.size(), 10);
        (void)readyCount;

        // run start method for each virtual server to handle requests/responses
        for (size_t i = 0; i < this->_virtualServers.size(); i++)
        {
            // each start method must
            //   - read request data
            //   - write response data
            // connections maybe an array of PollFd struct or someting that serve the same purpose!
            this->_virtualServers[i]->start(this->_connections);
        }
    }
}

Http::~Http()
{
    for (size_t i = 0; i < this->_virtualServers.size(); i++)
    {
        delete this->_virtualServers[i];
    }
}
