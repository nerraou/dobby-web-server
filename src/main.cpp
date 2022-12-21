#include <iostream>
#include "Server.hpp"
#include "ConfigHttp.hpp"
#include "ConfigServer.hpp"

int main(void)
{
    Server server;
    ConfigHttp httpContext;
    ConfigServer serverContext;

    try
    {
        std::cout << "Welcome to Dobby web server" << std::endl;
        serverContext.setPort(80);
        httpContext.addServerContext(serverContext);
        server.start(serverContext.getPort());
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}
