//
// Created by liu on 19-11-14.
//

#ifndef MYGC_TOOLS_H
#define MYGC_TOOLS_H

#ifdef __GNUC__
#define clz(x) __builtin_clzll(x)
#else
#define clz(x) static_assert(false && "no clz implementation found")
#endif

#include <cstdint>
#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <sstream>
#include <thread>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace mygc {
class Tools {
 public:
  static inline unsigned int getFirstZeroFromLeft(uint64_t n) {
    return clz(~n);
  }
  static inline unsigned int getLastOneFromRight(uint64_t n) {
    return 64 - clz(n);
  }
};

// not thread safe
class Log {
 public:
  Log(const char *format, ...) {
    std::stringstream stringstream;
    stringstream << now_str();
    stringstream << " ";
    stringstream << std::this_thread::get_id();
    stringstream << " ";
    fprintf(stderr, "%s", stringstream.str().c_str());
    va_list arg;
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    fprintf(stderr, "\n");
  }
 private:
  std::string now_str() {
    // Get current time from the clock, using microseconds resolution
    const boost::posix_time::ptime now =
        boost::posix_time::microsec_clock::local_time();

    // Get the time offset in current day
    const boost::posix_time::time_duration td = now.time_of_day();

    //
    // Extract hours, minutes, seconds and milliseconds.
    //
    // Since there is no direct accessor ".milliseconds()",
    // milliseconds are computed _by difference_ between total milliseconds
    // (for which there is an accessor), and the hours/minutes/seconds
    // values previously fetched.
    //
    const long hours = td.hours();
    const long minutes = td.minutes();
    const long seconds = td.seconds();
    const long milliseconds = td.total_milliseconds() -
        ((hours * 3600 + minutes * 60 + seconds) * 1000);

    //
    // Format like this:
    //
    //      hh:mm:ss.SSS
    //
    // e.g. 02:15:40:321
    //
    //      ^          ^
    //      |          |
    //      123456789*12
    //      ---------10-     --> 12 chars + \0 --> 13 chars should suffice
    //
    //
    char buf[40];
    sprintf(buf, "%02ld:%02ld:%02ld.%06ld",
            hours, minutes, seconds, milliseconds);

    return buf;
  }
};

}//namespace mygc
#endif //MYGC_TOOLS_H
