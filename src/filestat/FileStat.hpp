#pragma once

#include <string>
#include <fstream>
#include <sys/stat.h>
#include <cstring>

class FileStat
{
private:
    std::string _path;
    off_t _size;
    timespec _createdAt;
    timespec _updatedAt;
    timespec _lastAccessAt;
    mode_t _mode;

    void callStat();

public:
    FileStat();
    FileStat(const std::string &path);
    bool isFolder();
    off_t getSize() const;
    static FileStat open(const std::string &path);

    class FileStatException : public std::exception
    {
    protected:
        std::string _message;

    public:
        FileStatException(const std::string &message) throw();
        const char *what() const throw();
        ~FileStatException() throw();
    };
};
