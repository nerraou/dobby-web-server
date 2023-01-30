#include <iostream>
#include "Server.hpp"
#include "ConfigHttp.hpp"
#include "ConfigServer.hpp"
#include "Http.hpp"
#include "ParseConfig.hpp"
#include <csignal>

static void sig_child_handler(int sig)
{
    (void)sig;
}

static void initEnvVars(void)
{
    ::setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    ::setenv("SERVER_SOFTWARE", "dobby", 1);
    ::setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    ::unsetenv("CONTENT_LENGTH");
    ::unsetenv("CONTENT_TYPE");
}

int main(int ac, char *av[])
{
    ConfigHttp httpConfig;

    std::signal(SIGCHLD, sig_child_handler);
    std::signal(SIGPIPE, SIG_IGN);

    initEnvVars();

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
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << strerror(errno) << std::endl;
    }
    return (0);
}
