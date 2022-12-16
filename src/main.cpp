#include <iostream>
#include "ServerSocket.hpp"

int main(void)
{
    ServerSocket server;
    ServerSocket client;

    try
    {
        server.create();
        server.bind(8080);
        server.listen(100);
        server.accept(client);
        std::cout << "Welcome to Dobby web server" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}
