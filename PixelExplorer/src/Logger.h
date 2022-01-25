#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

class Logger {
 public:
  enum LogLevel { INFO, WARN, ERROR, DEBUG };

  static void Info(const char* msg) { Log(msg, INFO); }
  static void Warn(const char* msg) { Log(msg, WARN); }
  static void Error(const char* msg) { Log(msg, ERROR); }
  static void Debug(const char* msg) { Log(msg, DEBUG); }
  static void Info(const std::string msg) { Log(msg, INFO); }
  static void Warn(const std::string msg) { Log(msg, WARN); }
  static void Error(const std::string msg) { Log(msg, ERROR); }
  static void Debug(const std::string msg) { Log(msg, DEBUG); }
  static void Log(const std::string msg, LogLevel level) { Log(msg.c_str(), level); }
  static void Log(const char* msg, LogLevel level) {
    std::cout << "[" << level << "]: " << msg << std::endl;
  }

 private:
  Logger();
  ~Logger();
};
#endif  // !LOGGER_H
