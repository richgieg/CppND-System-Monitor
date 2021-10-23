#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using std::string;

string GetKernel();
string GetKernel2();
float MemoryUtilization();
int ReadFirstIntegerAfterKeyFromFile(string key, string file);

int main() {
  std::cout << MemoryUtilization() << "\n";
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

float MemoryUtilization() {
  int memTotal = ReadFirstIntegerAfterKeyFromFile("MemTotal:", "/proc/meminfo");
  int memFree = ReadFirstIntegerAfterKeyFromFile("MemFree:", "/proc/meminfo");
  int memUsed = memTotal - memFree;
  float memUtilizationPercent = static_cast<float>(memUsed) / memTotal * 100;
  return memUtilizationPercent;
}

int ReadFirstIntegerAfterKeyFromFile(string key, string file) {
  int firstInteger = 0;
  std::ifstream filestream(file);
  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line)) {
      std::cout << line << "\n";
      std::istringstream linestream(line);
      string lineKey;
      int value;
      linestream >> lineKey >> value;
      if (lineKey == key) {
        std::cout << value << "\n";
        firstInteger = value;
        break;
      }
    }
  }
  return firstInteger;
}
