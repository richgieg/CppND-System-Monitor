#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
  :pid_{pid},
  cpuUtilization_{static_cast<float>(LinuxParser::ActiveJiffies(pid)) / LinuxParser::Jiffies()},
  command_{LinuxParser::Command(pid)},
  ram_{LinuxParser::Ram(pid)},
  user_{LinuxParser::User(pid)},
  upTime_{LinuxParser::UpTime(pid)}
  {}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return cpuUtilization_; }

string Process::Command() { return command_; }

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

long int Process::UpTime() { return upTime_; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }