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

        Http http(httpConfig);
        http.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}
