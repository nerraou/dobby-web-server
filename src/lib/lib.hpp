#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <unistd.h>

namespace lib
{
    std::string trim(const std::string &line, const std::string &set = " \t");
    std::vector<std::string> split(const std::string &str, const std::string &set = " \t");
    void transform(std::string::iterator begin, std::string::iterator end, int (*transformer)(int c));
    std::size_t formatTime(char *buffer, std::size_t bufferSize, const std::string &format, const time_t timestamp);
    std::string normalizePath(const std::string &path);
    bool isFileExist(const std::string &path);

    template <typename T>
    std::string toString(T value)
    {
        std::stringstream stream;

        stream << value;

        return stream.str();
    }
}
