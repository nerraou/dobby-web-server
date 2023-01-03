#include "lib.hpp"

namespace lib
{
    std::string trim(const std::string &str, const std::string &set)
    {
        size_t start;
        size_t end;
        std::string trimmed;

        if (set.length() == 0)
            return (str);

        start = str.find_first_not_of(set);

        if (start == str.npos)
            trimmed = "";
        else
        {
            end = str.find_last_not_of(set);
            trimmed = str.substr(start, end - start + 1);
        }
        return (trimmed);
    }

    std::vector<std::string> split(const std::string &str, const std::string &set)
    {
        size_t startIndex = 0;
        size_t endIndex = 0;
        std::vector<std::string> strings;

        while (true)
        {
            startIndex = str.find_first_not_of(set, startIndex);
            if (startIndex != std::string::npos)
            {
                endIndex = str.find_first_of(set, startIndex);
                std::string temp;
                temp.append(str, startIndex, endIndex - startIndex);
                strings.push_back(temp);
                startIndex = endIndex;
                if (endIndex == std::string::npos)
                    break;
            }
            else
                break;
        }
        return strings;
    }
}
