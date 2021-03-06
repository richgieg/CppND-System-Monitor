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

long LinuxParser::ActiveJiffies(int pid) {
  constexpr int positionOfFirstValue = 14; // utime
  constexpr int positionOfLastValue = 17; // cstime
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  long activeJiffies = 0;
  if (filestream.is_open()) {
    string value;
    // Get utime, stime, cutime, and cstime (positions 14, 15, 16, and 17),
    // and add them together;
    for (int i = 0; i < positionOfLastValue; i++) {
      filestream >> value;
      if (i >= positionOfFirstValue - 1) {
        activeJiffies += std::stol(value);
      }
    }
  }
  return activeJiffies;
}

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

string LinuxParser::Command(int pid) {
  string command;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kCmdlineFilename);
  if (filestream.is_open()) {
    filestream >> command;
  }
  return command;
}

string LinuxParser::Ram(int pid) {
  constexpr int kilobytesPerMegabyte = 1024;
  // Using VmRSS, rather than VmSize, since it gives the exact physical
  // memory being used as a part of Physical RAM. VmSize is actually the
  // sum of all the virtual memory and can result in a value larger than
  // the system's physical RAM size.
  int vmRSS = ReadFirstIntegerAfterKeyFromFile("VmRSS:", kProcDirectory + to_string(pid) + kStatusFilename);
  int megabytesUsed = vmRSS / kilobytesPerMegabyte;
  return to_string(megabytesUsed);
}

string LinuxParser::Uid(int pid) {
  return to_string(ReadFirstIntegerAfterKeyFromFile("Uid:", kProcDirectory + to_string(pid) + kStatusFilename));
}

string LinuxParser::User(int pid) {
  auto uid = Uid(pid);
  string user;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string lineUser, lineX, lineUid;
      linestream >> lineUser >> lineX >> lineUid;
      if (lineUid == uid) {
        user = lineUser;
        break;
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) {
  constexpr int positionOfStartTime = 22;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  long startTimeClockTicks = 0;
  if (filestream.is_open()) {
    string value;
    for (int i = 0; i < positionOfStartTime; i++) {
      filestream >> value;
    }
    startTimeClockTicks = std::stol(value);
  }
  long startTimeSeconds = startTimeClockTicks / sysconf(_SC_CLK_TCK);
  long upTime = UpTime() - startTimeSeconds;
  return upTime;
}

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
        linestream >> cpuTimeInfo.user >> cpuTimeInfo.nice
          >> cpuTimeInfo.system >> cpuTimeInfo.idle
          >> cpuTimeInfo.iowait >> cpuTimeInfo.irq
          >> cpuTimeInfo.softirq >> cpuTimeInfo.steal
          >> cpuTimeInfo.guest >> cpuTimeInfo.guest_nice;
        break;
      }
    }
  }
  return cpuTimeInfo;
}
