#include "FileStat.hpp"

void FileStat::callStat()
{
    int statReturn;
    struct stat fileStat;

    statReturn = ::stat(this->_path.c_str(), &fileStat);
    if (statReturn == -1)
        throw FileStatException(strerror(errno));
    this->_size = fileStat.st_size;
    this->_createdAt = fileStat.STAT_MEMBER_CTIME;
    this->_updatedAt = fileStat.STAT_MEMBER_MTIME;
    this->_lastAccessAt = fileStat.STAT_MEMBER_ATIME;
    this->_mode = fileStat.st_mode;
}

FileStat::FileStat()
{
}

FileStat::FileStat(const std::string &path)
{
    this->_path = path;
    this->callStat();
}

bool FileStat::isFolder() const
{
    return (S_ISDIR(this->_mode));
}

off_t FileStat::getSize() const
{
    return this->_size;
}

FileStat FileStat::open(const std::string &path)
{
    FileStat fileStat(path);

    return fileStat;
}

FileStat::FileStatException::FileStatException(const std::string &message) throw()
{
    this->_message = message;
}

const char *FileStat::FileStatException::what() const throw()
{
    return this->_message.c_str();
}

FileStat::FileStatException::~FileStatException() throw()
{
}
