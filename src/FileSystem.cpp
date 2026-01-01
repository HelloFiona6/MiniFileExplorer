#include "FileSystem.h"

#include <dirent.h>
#include <sys/stat.h>

#include <ctime>
#include <iomanip>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <filesystem>
#include <system_error>
#include <filesystem>

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

unsigned long long FileSystem::calcDirSize(const std::string &path) {
    namespace fs = std::filesystem;
    unsigned long long total = 0;
    try {
        for (auto it = fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied);
             it != fs::recursive_directory_iterator(); ++it) {
            try {
                if (fs::is_regular_file(it->path())) {
                    total += fs::file_size(it->path());
                }
            } catch (...) {
                // ignore unreadable files
            }
        }
    } catch (...) {
        // ignore errors
    }
    return total;
}

void FileSystem::search(const std::string &path, const std::string &keyword, std::vector<std::pair<std::string,bool>> &results) {
    namespace fs = std::filesystem;
    auto toLower = [](const std::string &s){ std::string r = s; for (auto &c : r) c = static_cast<char>(::tolower(c)); return r; };
    std::string keyl = toLower(keyword);

    try {
        for (auto it = fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied);
             it != fs::recursive_directory_iterator(); ++it) {
            try {
                fs::path p = it->path();
                std::string name = p.filename().string();
                if (toLower(name).find(keyl) != std::string::npos) {
                    std::string display;
                    char resolved[PATH_MAX];
                    if (::realpath(p.string().c_str(), resolved)) display = resolved;
                    else display = p.string();
                    bool isDir = fs::is_directory(p);
                    if (isDir) display += "/";
                    results.emplace_back(display, isDir);
                }
            } catch (...) {
                // ignore entry
            }
        }
    } catch (...) {
        // ignore iteration errors
    }
}

bool FileSystem::copyFile(const std::string &src, const std::string &dst, bool overwrite) {
    namespace fs = std::filesystem;
    try {
        fs::path s(src);
        fs::path d(dst);

        if (!fs::exists(s)) return false;
        if (!fs::is_regular_file(s)) return false;

        if (fs::exists(d)) {
            if (fs::is_directory(d)) d /= s.filename();
            if (!overwrite) return false;
            fs::copy_file(s, d, fs::copy_options::overwrite_existing);
        } else {
            if (d.has_parent_path() && !fs::exists(d.parent_path())) return false;
            fs::copy_file(s, d);
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool FileSystem::move(const std::string &src, const std::string &dst, bool overwrite) {
    namespace fs = std::filesystem;
    try {
        fs::path s(src);
        fs::path d(dst);

        if (!fs::exists(s)) return false;

        if (fs::exists(d)) {
            if (fs::is_directory(d)) d /= s.filename();
            if (!overwrite) return false;
            if (fs::is_directory(d)) fs::remove_all(d);
            else fs::remove(d);
        } else {
            if (d.has_parent_path() && !fs::exists(d.parent_path())) return false;
        }

        std::error_code ec;
        fs::rename(s, d, ec);
        if (!ec) return true;

        // Fallback: copy then remove (handles cross-device moves)
        if (fs::is_regular_file(s)) {
            fs::copy_file(s, d, fs::copy_options::overwrite_existing);
            fs::remove(s);
        } else if (fs::is_directory(s)) {
            fs::copy(s, d, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            fs::remove_all(s);
        } else {
            return false;
        }

        return true;
    } catch (...) {
        return false;
    }
}


