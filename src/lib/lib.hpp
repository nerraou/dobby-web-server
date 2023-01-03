#pragma once
#include <string>
#include <vector>

namespace lib
{
    std::string trim(const std::string &line, const std::string &set = " \t");
    std::vector<std::string> split(const std::string &str, const std::string &set = " \t");
}
