#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
using namespace std;

#include <string>


struct FileInfo {
    string name;
    bool isDir;
    long size;
    string mtime;
};

class FileSystem {
public:
    static bool exists(const string& path);
    static bool isDir(const string& path);
};

#endif
