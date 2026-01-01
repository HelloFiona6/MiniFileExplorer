#include "commands/Commands.h"
#include "MiniFileExplorer.h"
#include "FileSystem.h"

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/stat.h>
#include <ctime>
#include <sstream>
#include <limits.h>
#include <functional>

// ----------------- Command Implementations -----------------

static void cmd_help()
{
    std::cout << "\n=== MiniFileExplorer Commands ===\n" << std::endl;
    std::cout << "Core Commands:" << std::endl;
    std::cout << "  ls [options]       - List contents of current directory" << std::endl;
    std::cout << "                     - Options: -s (sort by size), -t (sort by time)" << std::endl;
    std::cout << "  cd [path]          - Change current directory (e.g., cd ../docs)" << std::endl;
    std::cout << "  touch [filename]   - Create an empty file" << std::endl;
    std::cout << "  mkdir [dirname]    - Create a new directory" << std::endl;
    std::cout << "  rm [filename]      - Delete a file (with confirmation)" << std::endl;
    std::cout << "  rmdir [dirname]    - Delete an empty directory" << std::endl;
    std::cout << "  stat [name]        - Show detailed information of a file or directory" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Commands:" << std::endl;
    std::cout << "  search [keyword]   - Search files and directories recursively" << std::endl;
    std::cout << "  cp [src] [dst]     - Copy file from src to dst" << std::endl;
    std::cout << "  mv [src] [dst]     - Move or rename file or directory" << std::endl;
    std::cout << "  du [dirname]       - Show total size of directory" << std::endl;
    std::cout << std::endl;
    std::cout << "System:" << std::endl;
    std::cout << "  help               - Show this help message" << std::endl;
    std::cout << "  exit               - Exit MiniFileExplorer" << std::endl;
    std::cout << std::endl;
}

static void cmd_exit()
{
    std::cout << "MiniFileExplorer closed successfully\n";
    std::exit(0);
}

static void cmd_cd(MiniFileExplorer &app, const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << "Usage: cd [path]\n";
        return;
    }

    const std::string &path = args[1];

    if (!FileSystem::exists(path))
    {
        std::cout << "Invalid directory: " << path << "\n";
        return;
    }

    if (!FileSystem::isDir(path))
    {
        std::cout << "Not a directory: " << path << "\n";
        return;
    }

    if (::chdir(path.c_str()) != 0)
    {
        perror("cd");
        return;
    }

    char buf[1024];
    getcwd(buf, sizeof(buf));
    app.setCurrentDir(buf);
}

static void cmd_ls(MiniFileExplorer &app)
{
    /*
    TODO:
    显示路径
    文件结构存储
    */
    auto files = FileSystem::listDir(app.getCurrentDir());

    size_t nameWidth = 0;
    for (auto &f : files)
        nameWidth = std::max(nameWidth, f.name.size());

    // Print header
    std::cout << std::left << std::setw(nameWidth + 2) << "Name"
              << std::setw(8) << "Type"
              << std::setw(10) << "Size(B)"
              << "Modify Time" << "\n";

    // Separator
    std::cout << std::string(nameWidth + 2 + 8 + 10 + 20, '-') << "\n";

    for (auto &f : files)
    {
        std::string name = f.name + (f.isDir ? "/" : "");
        std::string type = f.isDir ? "Dir" : "File";
        std::string size = f.isDir ? "-" : std::to_string(f.size);

        std::cout << std::left << std::setw(nameWidth + 2) << name
                  << std::setw(8) << type
                  << std::setw(10) << size
                  << f.mtime << "\n";
    }
}

static void cmd_touch(const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << "Usage: touch <filename>\n";
        return;
    }

    if (FileSystem::exists(args[1]))
    {
        std::cout << "File already exists: " << args[1] << "\n";
        return;
    }

    if (!FileSystem::createFile(args[1]))
        std::cout << "Failed to create file: " << args[1] << "\n";
}

static void cmd_mkdir(const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << "Usage: mkdir <dirname>\n";
        return;
    }

    if (FileSystem::exists(args[1]))
    {
        std::cout << "Directory already exists: " << args[1] << "\n";
        return;
    }

    if (!FileSystem::createDir(args[1]))
        std::cout << "Failed to create directory: " << args[1] << "\n";
}

static void cmd_rm(const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << "Usage: rm <filename>\n";
        return;
    }

    std::string file = args[1];

    if (!FileSystem::exists(file))
    {
        std::cout << "File not found: " << file << "\n";
        return;
    }

    std::cout << "Are you sure to delete " << file << "? (y/n): ";
    std::string ans;
    std::getline(std::cin, ans);

    if (ans != "y")
        return;

    if (!FileSystem::removeFile(file))
        std::cout << "Failed to delete file: " << file << "\n";
}

static void cmd_rmdir(const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << "Usage: rmdir <dirname>\n";
        return;
    }

    std::string dir = args[1];

    if (!FileSystem::exists(dir))
    {
        std::cout << "Directory not found: " << dir << "\n";
        return;
    }

    if (!FileSystem::isDir(dir))
    {
        std::cout << "Not a directory: " << dir << "\n";
        return;
    }

    if (!FileSystem::isEmptyDir(dir))
    {
        std::cout << "Directory not empty: " << dir << "\n";
        return;
    }

    if (!FileSystem::removeDir(dir))
        std::cout << "Failed to remove directory: " << dir << "\n";
}

static void cmd_stat(const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << "Missingtarget: Please enter'stat [name]'" << std::endl;
        return;
    }

    const std::string path = args[1];

    if (!FileSystem::exists(path))
    {
        std::cout << "Targetnotfound:" << path << std::endl;
        return;
    }

    struct stat st{};
    if (::stat(path.c_str(), &st) != 0)
    {
        perror("stat");
        return;
    }

    bool is_dir = S_ISDIR(st.st_mode);
    std::string type = is_dir ? "Folder" : "File";

    // Resolve absolute path if possible
    char resolved[PATH_MAX];
    std::string fullpath;
    if (::realpath(path.c_str(), resolved))
        fullpath = resolved;
    else
        fullpath = path;

    std::string size = is_dir ? "-" : std::to_string(static_cast<long long>(st.st_size));

    auto fmt = [](time_t t) -> std::string {
        std::tm tm{};
        if (std::tm* p = std::localtime(&t)) tm = *p;
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    };

    std::cout << "=== File/Directory Information ===" << std::endl;
    std::cout << "Type: " << type << std::endl;
    std::cout << "Path: " << fullpath << std::endl;
    std::cout << "Size(B): " << size << std::endl;
    std::cout << "Creation Time: " << fmt(st.st_ctime) << std::endl;
    std::cout << "Modification Time: " << fmt(st.st_mtime) << std::endl;
    std::cout << "Access Time: " << fmt(st.st_atime) << std::endl;
}

static void cmd_search(MiniFileExplorer &app, const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << "Usage: search [keyword]" << std::endl;
        return;
    }

    const std::string keyword = args[1];
    // lowercase keyword for case-insensitive match
    auto toLower = [](const std::string &s) {
        std::string r = s;
        for (auto &c : r) c = static_cast<char>(::tolower(c));
        return r;
    };

    const std::string keyl = toLower(keyword);

    std::vector<std::pair<std::string, bool>> results; // (path, isDir)

    std::function<void(const std::string&)> dfs = [&](const std::string &base) {
        auto entries = FileSystem::listDir(base);
        for (auto &e : entries) {
            std::string entryPath = base + "/" + e.name;

            std::string nameL = toLower(e.name);
            if (nameL.find(keyl) != std::string::npos) {
                // resolve absolute path for display
                char resolved[PATH_MAX];
                std::string display;
                if (::realpath(entryPath.c_str(), resolved)) display = resolved;
                else display = entryPath;

                if (e.isDir) display += "/";
                results.emplace_back(display, e.isDir);
            }

            if (e.isDir) {
                // recurse into subdirectory
                dfs(entryPath);
            }
        }
    };

    std::string start = app.getCurrentDir();
    dfs(start);

    if (results.empty()) {
        std::cout << "No results found for '" << keyword << "'" << std::endl;
        return;
    }

    std::cout << "Search results for '" << keyword << "' (" << results.size() << " items):" << std::endl;
    for (auto &r : results) {
        std::cout << r.first << " (" << (r.second ? "Dir" : "File") << ")" << std::endl;
    }
}

void handleCommand(MiniFileExplorer &app, const std::vector<std::string> &args)
{
    if (args.empty())
        return;

    const std::string &cmd = args[0];

    if (cmd == "help")
        cmd_help();
    else if (cmd == "ls")
        cmd_ls(app);
    else if (cmd == "cd")
        cmd_cd(app, args);
    else if (cmd == "exit")
        cmd_exit();
    else if (cmd == "touch")
        cmd_touch(args);
    else if (cmd == "mkdir")
        cmd_mkdir(args);
    else if (cmd == "rm")
        cmd_rm(args);
    else if (cmd == "rmdir")
        cmd_rmdir(args);
    else if (cmd == "stat")
        cmd_stat(args);
    else if (cmd == "search")
    cmd_search(app, args);
    else
        std::cout << "Unknown command: " << cmd << "\n";
}