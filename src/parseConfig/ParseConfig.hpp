#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include "lib.hpp"

class ParseConfig
{
private:
    static std::string parseRoot(std::string &line);
    static size_t parseClientMaxBodySize(std::string &line);
    static size_t checkConvertUnit(int size,const char *str);
    static size_t convertToUnit(int size, const char unit);
    static bool parseAutoIndex(std::string &line);
    static std::vector<std::string> parseIndex(std::string &line);
    static void parseErrorPage();
    static void parseListen();
    static void parseServerName();
    static void parseAcceptedHttpMethods();
    static void parseRewrite();

public:
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
    static void parse(std::string configPath);
    ~ParseConfig();
};
