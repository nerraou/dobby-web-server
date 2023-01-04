#include <iostream>
#include "Server.hpp"
#include "ConfigHttp.hpp"
#include "ConfigServer.hpp"
#include "Http.hpp"
#include "ParseConfig.hpp"

int main(int ac, char *av[])
{
    // ConfigHttp http;
    std::vector<std::string> config;
    try
    {
        if (ac > 1)
            config = ParseConfig::loadConfigFile(av[1]);
        for (size_t i = 0; i < config.size(); i++)
            std::cout << config.at(i) << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return (0);
}
