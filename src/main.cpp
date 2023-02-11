#include <iostream>
#include <vector>
#include <csignal>

#include "Server.hpp"
#include "ConfigHttp.hpp"
#include "ConfigServer.hpp"
#include "Http.hpp"
#include "ParseConfig.hpp"
#include "ContentType.hpp"
#include "typedefs.hpp"

#define CONTENT_TYPE_PATH "./src/contentType/contentType.txt"

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
    std::string configPath;

    initEnvVars();

    try
    {
        if (ac != 2)
            configPath = DEFAULT_CONFIG_PATH;
        else
            configPath = av[1];

        ContentType::loadContentTypes(CONTENT_TYPE_PATH);
        httpConfig = ParseConfig::parse(configPath);

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
