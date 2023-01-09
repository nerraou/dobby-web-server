#include <iostream>
#include "Server.hpp"
#include "ConfigHttp.hpp"
#include "ConfigServer.hpp"
#include "Http.hpp"
#include "ParseConfig.hpp"

int main(int ac, char *av[])
{
    ConfigHttp httpConfig;

    try
    {
        if (ac != 2)
        {
            std::cerr << "usage: ./dobby config_file_path" << std::endl;
            return 1;
        }

        httpConfig = ParseConfig::parse(av[1]);

        if (httpConfig.isGood() == true)
        {
            Http http(httpConfig);
            http.start();
        }
        else
            std::cerr << "check config file" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return (0);
}
