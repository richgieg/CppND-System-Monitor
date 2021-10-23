#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU Time Info (jiffies).
struct CpuTimeInfo {
  // Time spent in user mode.
  long user;
  // Time spent in user mode with low priority (nice).
  long nice;
  // Time spent in system mode.
  long system;
  // Time spent in the idle task. This value should be
  // USER_HZ times the second entry in the /proc/uptime
  // pseudo-file.
  long idle;
  // Time waiting for I/O to complete (not reliable!).
  long iowait;
  // Time servicing interrupts (since Linux 2.6.0).
  long irq;
  // Time servicing softirqs (since Linux 2.6.0).
  long softirq;
  // Stolen time, which is the time spent in other
  // operating systems when running in a virtualized
  // environment (since Linux 2.6.11).
  long steal;
  // Time spent running a virtual CPU for guest operating
  // systems under the control of the Linux kernel
  // (since Linux 2.6.24).
  long guest;
  // Time spent running a niced guest (virtual CPU for
  // guest operating systems under the control of the Linux
  // kernel) (since Linux 2.6.33).
  long guest_nice;
};

float CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif