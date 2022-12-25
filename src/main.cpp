#include <iostream>
#include "Server.hpp"
#include "ConfigHttp.hpp"
#include "ConfigServer.hpp"
#include "Http.hpp"

int main(void)
{
    ConfigHttp httpContext;
    ConfigServer serverContext1;
    ConfigServer serverContext2;

    try
    {
        std::cout << "Welcome to Dobby web server" << std::endl;
        serverContext1.setPort(8081);
        serverContext2.setPort(8082);
        httpContext.addServerContext(serverContext1);
        httpContext.addServerContext(serverContext2);

        // attach http config to http instance
        Http http(httpContext);

        // start event loop
        http.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}
