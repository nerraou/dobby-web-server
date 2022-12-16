#include <iostream>
#include "Server.hpp"

int main(void)
{
    Server server;

    try
    {
        std::cout << "Welcome to Dobby web server" << std::endl;
        server.start(8080);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}
