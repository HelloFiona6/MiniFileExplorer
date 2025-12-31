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

void MiniFileExplorer::run() {
    std::cout << "Current Directory: " << currentDir << std::endl;

    while (true) {
        std::cout << "Enter command (type 'help' for all commands): ";
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
