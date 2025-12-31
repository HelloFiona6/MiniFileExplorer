#ifndef MINI_FILE_EXPLORER_H
#define MINI_FILE_EXPLORER_H

#include <string>

class MiniFileExplorer {
public:
    MiniFileExplorer();
    void run();
    void execute(const std::string& input);

    std::string getCurrentDir() const;
    void setCurrentDir(const std::string& path);

private:
    std::string currentDir;
};

#endif
