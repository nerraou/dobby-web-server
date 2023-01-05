#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include "lib.hpp"
#include "ConfigHttp.hpp"

class ParseConfig
{
private:
    static std::string parseRoot(std::string &line);
    static size_t parseClientMaxBodySize(std::string &line);
    static size_t checkConvertUnit(int size, const char *str);
    static size_t convertToUnit(int size, const char unit);
    static bool parseAutoIndex(std::string &line);
    static std::vector<std::string> parseIndex(std::string &line);
    static std::vector<std::string> parseErrorPage(std::string &line);
    static std::vector<std::string> parseServerName(std::string &line);
    static std::vector<std::string> parseAcceptedHttpMethods(std::string &line);
    static std::string parseRewrite(std::string &line);
    static int parseListen(std::string &line);
    static void parseHttpContext(std::string &line, ConfigHttp &httpContext);
    static void parseLocationContext(std::string &line, ConfigLocation &locationContext);
    static void parseServerContext(std::string &line, ConfigServer &serverContext);
    static void setContext(std::string &line, std::string &context);
    

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
