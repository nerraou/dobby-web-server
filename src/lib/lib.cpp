#include "lib.hpp"

namespace lib
{
    std::string trim(const std::string &str, const std::string &set)
    {
        size_t start;
        size_t end;
        std::string trimmed;

        if (set.empty())
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

    void transform(std::string::iterator begin, std::string::iterator end, int (*transformer)(int c))
    {
        while (begin != end)
        {
            *begin = transformer(*begin);
            begin++;
        }
    }

    std::string toString(int n)
    {
        char buffer[12] = {0};
        int index = 10;
        bool isNegative;

        if (n == 0)
            return std::string("0");
        isNegative = n < 0;
        while (n != 0)
        {
            if (n >= 0)
                buffer[index] = n % 10 + '0';
            else
                buffer[index] = -(n % 10) + '0';
            n /= 10;
            index--;
        }
        if (isNegative)
        {
            buffer[index] = '-';
            index--;
        }
        return std::string(buffer + index + 1);
    }

    std::size_t formatTime(char *buffer, std::size_t bufferSize, const std::string &format, const time_t timestamp)
    {
        const struct tm *time = ::localtime(&timestamp);

        return ::strftime(buffer, bufferSize, format.c_str(), time);
    }
}
