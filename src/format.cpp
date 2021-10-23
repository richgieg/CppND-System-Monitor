#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
  constexpr int secondsPerMinute = 60;
  constexpr int secondsPerHour = secondsPerMinute * 60;
  long hours, minutes;
  hours = seconds / secondsPerHour;
  seconds -= hours * secondsPerHour;
  minutes = seconds / secondsPerMinute;
  seconds -= minutes * secondsPerMinute;
  std::stringstream sstream;
  sstream << std::setfill('0')
    << std::setw(2) << hours << ":"
    << std::setw(2) << minutes << ":"
    << std::setw(2) << seconds;
  return sstream.str();
}