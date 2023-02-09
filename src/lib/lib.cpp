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

    std::size_t formatTime(char *buffer, std::size_t bufferSize, const std::string &format, const time_t timestamp)
    {
        const struct tm *time = ::localtime(&timestamp);

        return ::strftime(buffer, bufferSize, format.c_str(), time);
    }

    std::string normalizePath(const std::string &path)
    {
        std::stringstream normalizedPath;
        std::vector<std::string> pathParts;
        std::list<std::string> normalizedPathParts;
        bool hasTrainlingSlash;

        hasTrainlingSlash = path[path.size() - 1] == '/';
        pathParts = lib::split(path, "/");

        for (size_t i = 0; i < pathParts.size(); i++)
        {
            if (pathParts.at(i).compare(".") == 0)
                continue;
            if (pathParts.at(i).compare("..") == 0)
            {
                if (normalizedPathParts.empty())
                    throw std::invalid_argument("bad path");
                normalizedPathParts.pop_back();
            }
            else
                normalizedPathParts.push_back(pathParts.at(i));
        }

        if (path.at(0) == '/')
            normalizedPath << '/';

        while (!normalizedPathParts.empty())
        {
            normalizedPath << normalizedPathParts.front();

            normalizedPathParts.pop_front();

            if (!normalizedPathParts.empty() || hasTrainlingSlash)
                normalizedPath << '/';
        }
        return normalizedPath.str();
    }

    bool isFileExist(const std::string &path)
    {
        return (::access(path.c_str(), F_OK) == 0);
    }

    bool endsWith(const std::string &str, const std::string &ending)
    {
        if (ending.length() > str.length())
            return (false);
        return str.compare(str.length() - ending.length(), ending.length(), ending) == 0;
    }

    bool startsWith(const std::string &str, const std::string &prefix)
    {
        if (str.size() < prefix.size())
            return false;
        if (str.compare(0, prefix.size(), prefix) != 0)
            return false;
        return true;
    }

    int replaceDashWithUnderscore(int c)
    {
        if (c == '-')
            return '_';
        return c;
    }

    bool isMulOverflow(long long A, long long B)
    {
        if (A == 0 || B == 0)
            return false;
        long long result = A * B;
        if (A == result / B)
            return false;
        else
            return true;
    }
    int removeDirectory(const char *path)
    {
        DIR *dir;
        struct dirent *entry;
        int result;

        dir = opendir(path);
        if (dir == NULL)
            return -1;

        result = 0;
        while (result == 0 && (entry = readdir(dir)) != NULL)
        {
            if (::strcmp(entry->d_name, ".") == 0 || ::strcmp(entry->d_name, "..") == 0)
                continue;
            result = -1;
            const std::string &localPath = std::string(path) + '/' + std::string(entry->d_name);

            if (entry->d_type == DT_DIR)
                result = removeDirectory(localPath.c_str());
            else
                result = std::remove(localPath.c_str());
        }
        closedir(dir);

        if (result == 0)
            result = std::remove(path);
        return result;
    }

}
