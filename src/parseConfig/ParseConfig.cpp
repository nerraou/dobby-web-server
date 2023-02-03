#include "ParseConfig.hpp"

ParseConfig::ParseConfig()
{
}

ParseConfig::~ParseConfig()
{
}

void ParseConfig::parseHttpContext(const std::string &line, ConfigHttp &httpContext)
{
    size_t index;
    std::vector<std::string> parsedErrorPages;
    std::string errorPagePath;

    index = line.find_first_of(" \t");
    if (line.find("root ") == 0)
    {
        httpContext.setRoot(ParseConfig::parseRoot(line.substr(index)));
    }
    else if (line.find("client_max_body_size ") == 0)
    {
        httpContext.setClientMaxBodySize(ParseConfig::parseClientMaxBodySize(line.substr(index)));
    }
    else if (line.find("autoindex ") == 0)
    {
        httpContext.setAutoIndex(ParseConfig::parseAutoIndex(line.substr(index)));
    }
    else if (line.find("error_page ") == 0)
    {
        parsedErrorPages = ParseConfig::parseErrorPage(line.substr(index));
        errorPagePath = parsedErrorPages.back();
        parsedErrorPages.pop_back();

        httpContext.setErrorPagesFromList(parsedErrorPages, errorPagePath);
    }
    else if (line.find("index ") == 0)
    {
        httpContext.addIndex(ParseConfig::parseIndex(line.substr(index)));
    }
    else if (line.find("php_cgi_path ") == 0)
    {
        httpContext.setPHPCGIPath(ParseConfig::parseCGIDirective(line.substr(index)));
    }
    else
        throw ParseConfigException(std::string("unknown http directive: ").append(line));
}

void ParseConfig::parseServerContext(const std::string &line, ConfigServer &serverContext)
{
    size_t index;
    std::vector<std::string> parsedErrorPages;
    std::string errorPagePath;

    index = line.find_first_of(" \t");
    if (line.find("listen ") == 0)
    {
        serverContext.setPort(ParseConfig::parseListen(line.substr(index)));
    }
    else if (line.find("server_name ") == 0)
    {
        serverContext.addServerNames(ParseConfig::parseServerName(line.substr(index)));
    }
    else if (line.find("root ") == 0)
    {
        serverContext.setRoot(ParseConfig::parseRoot(line.substr(index)));
    }
    else if (line.find("error_page ") == 0)
    {
        parsedErrorPages = ParseConfig::parseErrorPage(line.substr(index));
        errorPagePath = parsedErrorPages.back();
        parsedErrorPages.pop_back();

        serverContext.setErrorPagesFromList(parsedErrorPages, errorPagePath);
    }
    else if (line.find("autoindex ") == 0)
    {
        serverContext.setAutoIndex(ParseConfig::parseAutoIndex(line.substr(index)));
    }
    else if (line.find("index ") == 0)
    {
        serverContext.addIndex(ParseConfig::parseIndex(line.substr(index)));
    }
    else if (line.find("client_max_body_size ") == 0)
    {
        serverContext.setClientMaxBodySize(ParseConfig::parseClientMaxBodySize(line.substr(index)));
    }
    else if (line.find("php_cgi_path ") == 0)
    {
        serverContext.setPHPCGIPath(ParseConfig::parseCGIDirective(line.substr(index)));
    }
    else
        throw ParseConfigException(std::string("unknown server directive: ").append(line));
}

void ParseConfig::parseLocationContext(const std::string &line, ConfigLocation &locationContext)
{
    size_t index;
    std::vector<std::string> parsedErrorPages;
    std::string errorPagePath;

    index = 0;
    index = line.find_first_of(" \t");
    if (line.find("path ") == 0)
    {
        locationContext.setPath(ParseConfig::parsePath(line.substr(index)));
    }
    else if (line.find("accepted_http_methods ") == 0)
    {
        locationContext.addAllowedHttpMethods(ParseConfig::parseAcceptedHttpMethods(line.substr(index)));
    }
    else if (line.find("rewrite ") == 0)
    {
        locationContext.setRewrite(ParseConfig::parseRewrite(line.substr(index)));
    }
    else if (line.find("root ") == 0)
    {
        locationContext.setRoot(ParseConfig::parseRoot(line.substr(index)));
    }
    else if (line.find("client_max_body_size ") == 0)
    {
        locationContext.setClientMaxBodySize(ParseConfig::parseClientMaxBodySize(line.substr(index)));
    }
    else if (line.find("autoindex ") == 0)
    {
        locationContext.setAutoIndex(ParseConfig::parseAutoIndex(line.substr(index)));
    }
    else if (line.find("index ") == 0)
    {
        locationContext.addIndex(ParseConfig::parseIndex(line.substr(index)));
    }
    else if (line.find("php_cgi_path ") == 0)
    {
        locationContext.setPHPCGIPath(ParseConfig::parseCGIDirective(line.substr(index)));
    }
    else if (line.find("error_page ") == 0)
    {
        parsedErrorPages = ParseConfig::parseErrorPage(line.substr(index));
        errorPagePath = parsedErrorPages.back();
        parsedErrorPages.pop_back();

        locationContext.setErrorPagesFromList(parsedErrorPages, errorPagePath);
    }
    else
        throw ParseConfigException(std::string("unknown location directive: ").append(line));
}

std::string ParseConfig::parseCGIDirective(const std::string &line)
{
    std::string path;

    path = lib::trim(line);
    if (path.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad cgi directive!"));
    return (path);
}

bool ParseConfig::setContext(std::string &line, std::string &context)
{
    std::string newContext;

    if (line.compare("http {") == 0)
        newContext = "http";
    else if (line.compare("server {") == 0)
        newContext = "server";
    else if (line.compare("location {") == 0)
        newContext = "location";
    else
        return false;

    if (newContext.compare("http") == 0 && !context.empty())
        throw ParseConfigException(std::string("can't have http nested inside ") + context);

    if (newContext.compare("server") == 0 && context.compare("http") != 0)
        throw ParseConfigException(std::string("can't have server nested inside ") + context);

    if (newContext.compare("location") == 0 && context.compare("server") != 0)
        throw ParseConfigException(std::string("can't have location nested inside ") + context);

    context = newContext;
    return true;
}

ConfigHttp ParseConfig::parse(std::string configPath)
{
    std::vector<std::string> configVector;
    size_t i;
    ConfigHttp httpContext;
    ConfigLocation locationContext;
    ConfigServer serverContext;
    std::string context;
    bool isNewServerContext;
    bool isNewLocationContext;

    isNewServerContext = true;
    isNewLocationContext = true;

    i = 0;
    configVector = ParseConfig::loadConfigFile(configPath);
    for (i = 0; i < configVector.size(); ++i)
    {
        if (ParseConfig::setContext(configVector[i], context))
            continue;

        if (context == "http")
        {
            if (configVector[i].compare("}") == 0)
            {
                if (i != configVector.size() - 1)
                    throw ParseConfigException("bad config file");

                // check if http is good then return it else throw exception
                if (httpContext.isGood() == false)
                    throw ParseConfigException("bad http context");
                return httpContext;
            }
            ParseConfig::parseHttpContext(configVector[i], httpContext);
        }
        else if (context == "server")
        {
            if (isNewServerContext)
            {
                serverContext.inherit(httpContext);
                isNewServerContext = false;
            }

            if (configVector[i] == "}")
            {
                isNewServerContext = true;
                if (serverContext.isGood() == true)
                {
                    // change context to the parent ; context = http
                    context = "http";
                    httpContext.addServerContext(serverContext);
                }
                else
                    throw ParseConfigException("bad server context");
            }
            else
                ParseConfig::parseServerContext(configVector[i], serverContext);
        }
        else if (context == "location")
        {
            if (isNewLocationContext)
            {
                locationContext.inherit(serverContext);
                isNewLocationContext = false;
            }

            if (configVector[i] == "}")
            {
                isNewLocationContext = true;
                if (locationContext.isGood() == true)
                {
                    // call isGood location and add the location to server context
                    // change context to the parent ; context = server
                    context = "server";
                    serverContext.addLocationContext(locationContext);
                    serverContext.sortLocationByPath();
                }
                else
                    throw ParseConfigException("bad location context");
            }
            else
                ParseConfig::parseLocationContext(configVector[i], locationContext);
        }
    }

    throw ParseConfigException("bad config file");
}

std::string ParseConfig::parseRoot(const std::string &line)
{
    std::string root;

    root = lib::trim(line);
    if (root.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad root!"));
    return (root);
}

size_t ParseConfig::convertToUnit(int size, const char unit)
{
    if (unit == 'm')
        return size * 1000000;
    else
        return size * 1000;
}

size_t ParseConfig::checkConvertUnit(int size, const char *str)
{
    int len;
    int unit;

    if (str[0] == '\0')
        return size;
    len = ::strlen(str);
    if (len > 1)
        return (0);
    unit = tolower(str[0]);
    if ((char)unit == 'm' || (char)unit == 'k')
    {
        return convertToUnit(size, unit);
    }
    return (0);
}

size_t ParseConfig::parseClientMaxBodySize(const std::string &line)
{
    size_t size;
    int num;
    char *pEnd;
    std::string trimLine;

    trimLine = lib::trim(line);
    if (trimLine.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad max body size!"));
    num = (int)std::strtol(trimLine.c_str(), &pEnd, 10);
    if (num <= 0)
        throw(ParseConfig::ParseConfigException("Error Bad max body size!"));
    size = ParseConfig::checkConvertUnit(num, pEnd);
    if (size == 0)
        throw(ParseConfig::ParseConfigException("Error Bad max body size unit!"));
    return size;
}

int ParseConfig::parseListen(const std::string &line)
{
    int listen;
    char *pEnd;

    std::string trimLine;
    trimLine = lib::trim(line);
    if (trimLine.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad Listen!"));
    listen = (int)std::strtol(trimLine.c_str(), &pEnd, 10);
    if (listen <= 0 || listen > 65535 || pEnd[0] != '\0')
        throw(ParseConfig::ParseConfigException("Error Bad Listen!"));
    return listen;
}

std::vector<std::string> ParseConfig::loadConfigFile(std::string configPath)
{
    std::ifstream file(configPath.c_str());
    std::string line;
    std::vector<std::string> configVector;

    if (file.is_open() == false)
        throw(ParseConfig::FileException());
    while (std::getline(file, line))
    {
        line = lib::trim(line);
        if (line.size() != 0 && line[0] != '#')
            configVector.push_back(line);
    }
    return configVector;
}

bool ParseConfig::parseAutoIndex(const std::string &line)
{
    std::string trimLine;

    trimLine = lib::trim(line);

    if (std::strcmp(trimLine.c_str(), "on") == 0)
        return (true);
    else if (std::strcmp(trimLine.c_str(), "off") == 0)
        return false;
    else
        throw(ParseConfig::ParseConfigException("Error Bad AutoIndex"));
}

std::vector<std::string> ParseConfig::parseIndex(const std::string &line)
{
    std::string trimLine;

    trimLine = lib::trim(line);
    if (trimLine.empty())
        throw(ParseConfig::ParseConfigException("Error Bad Index"));
    return lib::split(trimLine);
}

std::vector<std::string> ParseConfig::parseErrorPage(const std::string &line)
{
    std::string trimLine;
    std::vector<std::string> errorPages;

    trimLine = lib::trim(line);
    if (trimLine.length() == 0)
        throw(ParseConfig::ParseConfigException("Error Bad Error page"));
    errorPages = lib::split(trimLine);
    if (errorPages.size() < 2)
     throw(ParseConfig::ParseConfigException("Error Bad Error page"));
    return errorPages;
}

std::vector<std::string> ParseConfig::parseServerName(const std::string &line)
{
    std::string trimLine;

    trimLine = lib::trim(line);
    if (trimLine.empty())
        throw(ParseConfig::ParseConfigException("Error Bad Server name"));
    return lib::split(trimLine);
}

std::vector<std::string> ParseConfig::parseAcceptedHttpMethods(const std::string &line)
{
    std::string trimLine;

    trimLine = lib::trim(line);
    if (trimLine.empty())
        throw(ParseConfig::ParseConfigException("Error Bad Http methods"));
    return lib::split(trimLine);
}

std::vector<std::string> ParseConfig::parseRewrite(const std::string &line)
{
    std::string trimLine;
    std::vector<std::string> rewrite;

    trimLine = lib::trim(line);
    if (trimLine.empty())
        throw(ParseConfig::ParseConfigException("Error Bad Rewrite"));
    rewrite = lib::split(trimLine);
    if (rewrite.size() != 2)
        throw(ParseConfig::ParseConfigException("Error Bad Rewrite"));
    return rewrite;
}

std::string ParseConfig::parsePath(const std::string &line)
{
    std::string trimLine;

    trimLine = lib::trim(line);
    if (trimLine.empty())
        throw(ParseConfig::ParseConfigException("Error Bad Path"));
    return trimLine;
}

ParseConfig::ParseConfigException::ParseConfigException(const std::string &message)
{
    this->_message = message;
}

const char *ParseConfig::ParseConfigException::what() const throw()
{
    return this->_message.c_str();
}

ParseConfig::ParseConfigException::~ParseConfigException() throw()
{
}

const char *ParseConfig::FileException::what() const throw()
{
    return ("Error while trying to open the file.");
}

ParseConfig::FileException::~FileException() throw()
{
}
