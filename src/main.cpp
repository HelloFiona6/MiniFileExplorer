#include "MiniFileExplorer.h"
#include "FileSystem.h"

#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[]) {
    std::string startDir;

    if (argc == 1) {
        char buf[1024];
        getcwd(buf, sizeof(buf));
        startDir = buf;
    } else {
        startDir = argv[1];

        if (!FileSystem::exists(startDir)) {
            std::cout << "Directory not found: " << startDir << "\n";
            return 1;
        }

        if (!FileSystem::isDir(startDir)) {
            std::cout << "Not a directory: " << startDir << "\n";
            return 1;
        }
    }

    MiniFileExplorer app(startDir);
    app.run();
    return 0;
}