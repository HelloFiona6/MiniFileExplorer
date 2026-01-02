#include "MiniFileExplorer.h"
#include "commands/Commands.h"
#include "Utils.h"

#include <iostream>
#include <unistd.h>
#include <limits.h>

MiniFileExplorer::MiniFileExplorer() {
    char buf[PATH_MAX];
    getcwd(buf, sizeof(buf));
    currentDir = buf;
}

MiniFileExplorer::MiniFileExplorer(const std::string& startDir) {
    currentDir = startDir;
}


void MiniFileExplorer::run() {
    std::cout << "Current Directory: " << currentDir << std::endl;

    ::chdir(currentDir.c_str());

    while (true) {
        char buf[1024];
        getcwd(buf, sizeof(buf));

        const char* BLUE  = "\033[1;34m";
        const char* GREEN = "\033[1;32m";
        const char* WHITE = "\033[0m";

        std::cout << BLUE << "[Enter command (type 'help' for all commands):] "
                  << GREEN << buf
                  << WHITE << " > ";

        std::string input;
        std::getline(std::cin, input);

        auto args = split(input);
        if (args.empty()) continue;

        execute(input);
    }
}

void MiniFileExplorer::execute(const std::string& input) {
    auto args = split(input);
    handleCommand(*this, args);
}

std::string MiniFileExplorer::getCurrentDir() const {
    return currentDir;
}

void MiniFileExplorer::setCurrentDir(const std::string& path) {
    currentDir = path;
}
