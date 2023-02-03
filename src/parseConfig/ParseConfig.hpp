#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include "lib.hpp"
#include "ConfigHttp.hpp"

class ParseConfig
{
private:
    static std::string parseRoot(const std::string &line);
    static size_t parseClientMaxBodySize(const std::string &line);
    static size_t checkConvertUnit(int size, const char *str);
    static size_t convertToUnit(int size, const char unit);
    static bool parseAutoIndex(const std::string &line);
    static std::vector<std::string> parseIndex(const std::string &line);
    static std::vector<std::string> parseErrorPage(const std::string &line);
    static std::vector<std::string> parseServerName(const std::string &line);
    static std::vector<std::string> parseAcceptedHttpMethods(const std::string &line);
    static std::vector<std::string> parseRewrite(const std::string &line);
    static std::string parsePath(const std::string &line);
    static int parseListen(const std::string &line);
    static void parseHttpContext(const std::string &line, ConfigHttp &httpContext);
    static void parseLocationContext(const std::string &line, ConfigLocation &locationContext);
    static void parseServerContext(const std::string &line, ConfigServer &serverContext);
    static std::string parseCGIDirective(const std::string &line);
    static bool setContext(std::string &line, std::string &context);

public:
    static std::vector<std::string> loadConfigFile(std::string configPath);
    ParseConfig();
    class FileException : public std::exception
    {
    public:
        virtual const char *what() const throw();
        virtual ~FileException() throw();
    };

    class ParseConfigException : public std::exception
    {
    private:
        std::string _message;

    public:
        ParseConfigException(const std::string &message);
        virtual const char *what() const throw();
        virtual ~ParseConfigException() throw();
    };
    static ConfigHttp parse(std::string configPath);
    ~ParseConfig();
};
