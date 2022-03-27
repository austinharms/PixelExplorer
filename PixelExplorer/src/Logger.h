#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace px {
class Logger {
 public:
  enum LogLevel { LOGINFO, LOGWARN, LOGERROR, LOGDEBUG, LOGFATAL };

  static void info(const char* msg);
  static void warn(const char* msg);
  static void error(const char* msg);
  static void debug(const char* msg);
  static void fatal(const char* msg);
  static void info(const std::string msg);
  static void warn(const std::string msg);
  static void error(const std::string msg);
  static void debug(const std::string msg);
  static void fatal(const std::string msg);
  static void log(const std::string msg, LogLevel level);
  static void log(const char* msg, LogLevel level);

 private:
  static const char* toLevelString(LogLevel level);
  Logger();
  ~Logger();
};
}  // namespace px
#endif  // !LOGGER_H
