#include "FileSystem.h"

#include <dirent.h>
#include <sys/stat.h>

#include <ctime>
#include <iomanip>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

bool FileSystem::exists(const std::string& path) {
    struct stat st{};
    return ::stat(path.c_str(), &st) == 0;
}

bool FileSystem::isDir(const std::string& path) {
    struct stat st{};
    if (::stat(path.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

std::vector<FileInfo> FileSystem::listDir(const std::string& path) {
    std::vector<FileInfo> result;

    DIR* dir = ::opendir(path.c_str());
    if (!dir) return result;

    while (dirent* entry = ::readdir(dir)) {
        const char* name = entry->d_name;
        if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
            continue; // skip "." and ".."

        const std::string fullPath = path + "/" + name;

        struct stat st{};
        if (::stat(fullPath.c_str(), &st) != 0) continue;

        FileInfo info;
        info.name = name;
        info.isDir = S_ISDIR(st.st_mode);
        info.size = info.isDir ? -1 : static_cast<std::int64_t>(st.st_size);

        std::tm tm{};
        std::time_t t = st.st_mtime;
        if (std::tm* p = std::localtime(&t)) tm = *p;

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        info.mtime = oss.str();

        result.push_back(std::move(info));
    }

    ::closedir(dir);
    return result;
}

bool FileSystem::createFile(const std::string& path) {
    int fd = ::open(path.c_str(), O_CREAT | O_EXCL, 0644);
    if (fd < 0) return false;
    ::close(fd);
    return true;
}

bool FileSystem::createDir(const std::string& path) {
    return ::mkdir(path.c_str(), 0755) == 0;
}

bool FileSystem::removeFile(const std::string& path) {
    return ::unlink(path.c_str()) == 0;
}

bool FileSystem::isEmptyDir(const std::string& path) {
    DIR* dir = ::opendir(path.c_str());
    if (!dir) return false;

    while (auto* entry = ::readdir(dir)) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            ::closedir(dir);
            return false;
        }
    }
    ::closedir(dir);
    return true;
}

bool FileSystem::removeDir(const std::string& path) {
    return ::rmdir(path.c_str()) == 0;
}


