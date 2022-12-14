#pragma once
#include <string>
#include <vector>

namespace lib
{
    std::string trim(const std::string &line, const std::string &set = " \t");
    std::vector<std::string> split(const std::string &str, const std::string &set = " \t");
    void transform(std::string::iterator begin, std::string::iterator end, int (*transformer)(int c));
    std::string toString(int n);
    std::size_t formatTime(char *buffer, std::size_t bufferSize, const std::string &format, const time_t timestamp);
}
