#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using std::string;

string GetKernel();
string GetKernel2();

int main() {
  string kernel_version = GetKernel2();
  std::cout << kernel_version << "\n";
}

string GetKernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream("/proc/version");
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

string GetKernel2() {
  string os, kernel, version;
  string line;
  std::ifstream stream("/proc/version");
  if (stream.is_open()) {
    stream >> os >> version >> kernel;
  }
  return kernel;
}
