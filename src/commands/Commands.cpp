#include "commands/Commands.h"
#include "MiniFileExplorer.h"
#include "FileSystem.h"

#include <iostream>
#include <iomanip>
#include <unistd.h>

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

    for (auto &f : files)
    {
        std::string name = f.name + (f.isDir ? "/" : "");

        std::cout << std::left << std::setw(nameWidth + 2) << name
                  << std::setw(10) << (f.isDir ? "-" : std::to_string(f.size))
                  << std::setw(6) << (f.isDir ? "Dir" : "File")
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
    else
        std::cout << "Unknown command: " << cmd << "\n";
}