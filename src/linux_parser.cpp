#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Internal helper function that reads the first integer after
// the given key from the given file.
int ReadFirstIntegerAfterKeyFromFile(string key, string file);

// Internal helper function that reads CPU time info (jiffies)
// from the stat file.
LinuxParser::CpuTimeInfo ReadCpuTimeInfoFromStatFile();

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
// MODIFIED: Read strings directly from ifstream rather than
// creating an unnecessary istringstream instance.
string LinuxParser::Kernel() {
  string os, kernel, version;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    stream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  int memTotal = ReadFirstIntegerAfterKeyFromFile("MemTotal:", kProcDirectory + kMeminfoFilename);
  int memFree = ReadFirstIntegerAfterKeyFromFile("MemFree:", kProcDirectory + kMeminfoFilename);
  int memUsed = memTotal - memFree;
  // Cast at least one operand to float so that fractional part of
  // quotient isn't truncated.
  float memUtilization = static_cast<float>(memUsed) / memTotal;
  return memUtilization;
}

long LinuxParser::UpTime() {
  long uptimeSeconds = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    stream >> uptimeSeconds;
  }
  return uptimeSeconds;
}

long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

long LinuxParser::ActiveJiffies() {
  auto cpuTimeInfo = ReadCpuTimeInfoFromStatFile();
  return cpuTimeInfo.user + cpuTimeInfo.nice + cpuTimeInfo.system
    + cpuTimeInfo.irq + cpuTimeInfo.softirq;
}

long LinuxParser::IdleJiffies() {
  auto cpuTimeInfo = ReadCpuTimeInfoFromStatFile();
  return cpuTimeInfo.idle;
}

float LinuxParser::CpuUtilization() {
  auto totalJiffies = Jiffies();
  auto activeJiffies = ActiveJiffies();
  // Cast at least one operand to float so that fractional part of
  // quotient isn't truncated.
  float cpuUtilization = static_cast<float>(activeJiffies) / totalJiffies;
  return cpuUtilization;
}

int LinuxParser::TotalProcesses() {
  return ReadFirstIntegerAfterKeyFromFile("processes", kProcDirectory + kStatFilename);
}

int LinuxParser::RunningProcesses() {
  return ReadFirstIntegerAfterKeyFromFile("procs_running", kProcDirectory + kStatFilename);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }

int ReadFirstIntegerAfterKeyFromFile(string key, string file) {
  int firstInteger = 0;
  std::ifstream filestream(file);
  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string lineKey;
      int value;
      linestream >> lineKey >> value;
      if (lineKey == key) {
        firstInteger = value;
        break;
      }
    }
  }
  return firstInteger;
}

LinuxParser::CpuTimeInfo ReadCpuTimeInfoFromStatFile() {
  LinuxParser::CpuTimeInfo cpuTimeInfo = {};
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string key;
      linestream >> key;
      if (key == "cpu") {
        linestream >> cpuTimeInfo.user;
        linestream >> cpuTimeInfo.nice;
        linestream >> cpuTimeInfo.system;
        linestream >> cpuTimeInfo.idle;
        linestream >> cpuTimeInfo.iowait;
        linestream >> cpuTimeInfo.irq;
        linestream >> cpuTimeInfo.softirq;
        linestream >> cpuTimeInfo.steal;
        linestream >> cpuTimeInfo.guest;
        linestream >> cpuTimeInfo.guest_nice;
        break;
      }
    }
  }
  return cpuTimeInfo;
}
