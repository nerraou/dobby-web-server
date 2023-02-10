#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <cstdio>

namespace lib
{
    std::string trim(const std::string &line, const std::string &set = " \t");
    std::string trimStart(const std::string &line, const std::string &set = " \t");
    std::vector<std::string> split(const std::string &str, const std::string &set = " \t");
    void transform(std::string::iterator begin, std::string::iterator end, int (*transformer)(int c));
    std::size_t formatTime(char *buffer, std::size_t bufferSize, const std::string &format, const time_t timestamp);
    std::string normalizePath(const std::string &path);
    bool isFileExist(const std::string &path);
    bool endsWith(const std::string &str, const std::string &ending);
    bool startsWith(const std::string &str, const std::string &prefix);
    int replaceDashWithUnderscore(int c);
    bool isMulOverflow(long long A, long long B);
    int removeDirectory(const char *path);

    template <typename T>
    std::string toString(T value)
    {
        std::stringstream stream;

        stream << value;

        return stream.str();
    }

    template <typename InputIt, typename UnaryPredicate>
    InputIt findIfNot(InputIt first, InputIt last, UnaryPredicate q)
    {
        for (; first != last; ++first)
        {
            if (!q(*first))
                return first;
        }
        return (last);
    }
}
