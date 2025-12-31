#include "commands/Commands.h"
#include "FileSystem.h"

#include <iostream>
#include <unistd.h>

void handleCommand(MiniFileExplorer& app, const std::vector<std::string>& args) {
    const std::string& cmd = args[0];

    if (cmd == "help") {
        std::cout << "Supported commands:\n";
        std::cout << "  help   Show this help message\n";
        std::cout << "  cd     Change directory\n";
        std::cout << "  exit   Exit the program\n";
    }
    else if (cmd == "cd") {
        if (args.size() < 2) {
            std::cout << "Usage: cd [path]\n";
            return;
        }

        std::string path = args[1];

        if (!FileSystem::exists(path)) {
            std::cout << "Invalid directory: " << path << std::endl;
            return;
        }

        if (!FileSystem::isDir(path)) {
            std::cout << "Not a directory: " << path << std::endl;
            return;
        }

        chdir(path.c_str());
        char buf[1024];
        getcwd(buf, sizeof(buf));
        app.setCurrentDir(buf);
    }
    else if (cmd == "exit") {
        std::cout << "MiniFileExplorer closed successfully\n";
        exit(0);
    }
    else {
        std::cout << "Unknown command: " << cmd << std::endl;
    }
}
