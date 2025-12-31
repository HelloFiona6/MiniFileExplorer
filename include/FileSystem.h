#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>

class FileSystem {
public:
    static bool exists(const std::string& path);
    static bool isDir(const std::string& path);
};

#endif
