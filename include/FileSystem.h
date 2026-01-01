#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>
#include <cstdint>
#include <utility>

struct FileInfo
{
    std::string name;  // entry name (no trailing '/')
    bool isDir;        // true if directory
    std::int64_t size; // -1 for directory, otherwise bytes
    std::string mtime; // "YYYY-MM-DD HH:MM:SS"
};

class FileSystem
{
public:
    static bool exists(const std::string &path);
    static bool isDir(const std::string &path);
    static std::vector<FileInfo> listDir(const std::string &path);
    static bool createFile(const std::string &path);
    static bool createDir(const std::string &path);
    static bool removeFile(const std::string &path);
    static bool removeDir(const std::string &path);
    static bool isEmptyDir(const std::string &path);
    static bool copyFile(const std::string &src, const std::string &dst, bool overwrite = false);
    static bool move(const std::string &src, const std::string &dst, bool overwrite = false);
    static unsigned long long calcDirSize(const std::string &path);
    static void search(const std::string &path, const std::string &keyword, std::vector<std::pair<std::string,bool>> &results);
    // static FileInfo getInfo(const std::string &path);
};

#endif
