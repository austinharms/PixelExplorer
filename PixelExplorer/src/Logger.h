#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

class Logger {
 public:
  enum LogLevel { LOGINFO, LOGWARN, LOGERROR, LOGDEBUG, LOGFATAL };

  static void Info(const char* msg);
  static void Warn(const char* msg);
  static void Error(const char* msg);
  static void Debug(const char* msg);
  static void Fatal(const char* msg);
  static void Info(const std::string msg);
  static void Warn(const std::string msg);
  static void Error(const std::string msg);
  static void Debug(const std::string msg);
  static void Fatal(const std::string msg);
  static void Log(const std::string msg, LogLevel level);
  static void Log(const char* msg, LogLevel level);

 private:
  static const char* toLevelString(LogLevel level);
  Logger();
  ~Logger();
};
#endif  // !LOGGER_H
