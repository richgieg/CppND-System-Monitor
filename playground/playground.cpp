#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

std::string GetKernel();
std::string GetKernel2();

int main() {
    std::string kernel_version = GetKernel2();
    std::cout << kernel_version << "\n";
}

std::string GetKernel() {
    std::string os, kernel, version;
    std::string line;
    std::ifstream stream("/proc/version");
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
    }
    return kernel;
}

std::string GetKernel2() {
    std::string os, kernel, version;
    std::string line;
    std::ifstream stream("/proc/version");
    if (stream.is_open()) {
        stream >> os >> version >> kernel;
    }
    return kernel;
}
