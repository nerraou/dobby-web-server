#pragma once

#include <string>
#include <exception>

class Url
{
private:
    void parseFragment(std::string &urlString);

    void parseQueryString(std::string &urlString);

    void parseScheme(std::string &urlString);

    void parsePath(std::string &urlString);

    void parsePort(std::string &urlString);

    void parseHost(std::string &urlString);

public:
    std::string scheme;
    std::string host;
    std::string path;
    std::string fragment;
    std::string queryString;
    std::string port;

    static Url parse(const std::string &urlString);

    class ParseUrlException : public std::exception
    {
    protected:
        std::string _message;

    public:
        ParseUrlException(const std::string &messsage) throw();

        const char *what() const throw();

        virtual ~ParseUrlException() throw();
    };
};
