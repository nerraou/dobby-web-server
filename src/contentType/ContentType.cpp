#include "ContentType.hpp"

std::map<std::string, std::string> ContentType::contentTypes;

void ContentType::loadContentTypes(const std::string &contentTypePath)
{
    std::ifstream path(contentTypePath.c_str());
    std::string line;
    std::vector<std::string> strings;

    while (std::getline(path, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        strings = lib::split(line, " :");
        if (strings.size() != 2)
            throw std::invalid_argument("bad line");
        ContentType::contentTypes.insert(std::make_pair(strings[0], strings[1]));
    }
    path.close();
}
