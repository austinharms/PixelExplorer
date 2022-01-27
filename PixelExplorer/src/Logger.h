#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

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
  static void Log(const std::string msg, LogLevel level) { Log(msg.c_str(), level); }
  static void Log(const char* msg, LogLevel level) {
    std::cout << "[" << level << "]: " << msg << std::endl;
  }

 private:
  Logger();
  ~Logger();
};
#endif  // !LOGGER_H
