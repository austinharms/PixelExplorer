#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

class Logger {
 public:
  enum LogLevel { LOGINFO, LOGWARN, LOGERROR, LOGDEBUG };

  static void Info(const char* msg) { Log(msg, LOGINFO); }
  static void Warn(const char* msg) { Log(msg, LOGWARN); }
  static void Error(const char* msg) { Log(msg, LOGERROR); }
  static void Debug(const char* msg) { Log(msg, LOGDEBUG); }
  static void Info(const std::string msg) { Log(msg, LOGINFO); }
  static void Warn(const std::string msg) { Log(msg, LOGWARN); }
  static void Error(const std::string msg) { Log(msg, LOGERROR); }
  static void Debug(const std::string msg) { Log(msg, LOGDEBUG); }
  static void Log(const std::string msg, LogLevel level) {
    Log(msg.c_str(), level);
  }
  static void Log(const char* msg, LogLevel level) {
    std::cout << "[" << toLevelString(level) << "]: " << msg << std::endl;
  }

 private:
  static const char* toLevelString(LogLevel level) {
    switch (level) { case LOGINFO:
        return "Info";
      case LOGWARN:
        return "Warn";
      case LOGERROR:
        return "Error";
      case LOGDEBUG:
        return "Debug";
      default:
        return "Unknown";
    }
  }
  Logger();
  ~Logger();
};
#endif  // !LOGGER_H
