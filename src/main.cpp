#include <iostream>
#include "Server.hpp"
#include "ConfigHttp.hpp"
#include "ConfigServer.hpp"
#include "Http.hpp"
#include "ParseConfig.hpp"

int main(int ac, char *av[])
{
    ConfigHttp http;
    std::vector<std::string> config;
    try
    {
        if (ac > 1)
            http = ParseConfig::parse(av[1]);
        if (http.isGood() == true)
            std::cout << "http is good \n";
        else 
            std::cout << "http is not good \n";

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return (0);
}
