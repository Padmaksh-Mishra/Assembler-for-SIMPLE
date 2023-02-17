#include <iostream>
#include <sstream>
#include <vector>

std::vector<std::string> split(const std::string& str) {
    std::vector<std::string> result;
    std::istringstream iss(str);
    for (std::string s; iss >> s; ) {
        result.push_back(s);
    }
    return result;
}

int main() {
    std::string input = " am jk";
    std::vector<std::string> output = split(input);
    for (const std::string& s : output) {
        std::cout << s << '\n';
    }
    return 0;
}
