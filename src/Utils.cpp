#include "Utils.h"
#include <sstream>

std::vector<std::string> split(const std::string& input) {
    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> res;

    while (ss >> token)
        res.push_back(token);

    return res;
}
