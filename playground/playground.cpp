#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

std::string GetKernelVersion();
std::string GetKernelVersion2();

int main() {
    std::string kernel_version = GetKernelVersion2();
    std::cout << kernel_version << "\n";
}

std::string GetKernelVersion() {
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

std::string GetKernelVersion2() {
    std::string os, kernel, version;
    std::string line;
    std::ifstream stream("/proc/version");
    if (stream.is_open()) {
        stream >> os >> version >> kernel;
    }
    return kernel;
}
