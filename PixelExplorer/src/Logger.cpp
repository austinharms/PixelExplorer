#include "Logger.h"

void Logger::Info(const char* msg) { Log(msg, LOGINFO); }
void Logger::Warn(const char* msg) { Log(msg, LOGWARN); }
void Logger::Error(const char* msg) { Log(msg, LOGERROR); }
void Logger::Debug(const char* msg) { Log(msg, LOGDEBUG); }
void Logger::Fatal(const char* msg) {
  Log(msg, LOGFATAL);
#ifndef DEBUG
  exit(1);
#endif  // !DEBUG
}

void Logger::Info(const std::string msg) { Log(msg, LOGINFO); }
void Logger::Warn(const std::string msg) { Log(msg, LOGWARN); }
void Logger::Error(const std::string msg) { Log(msg, LOGERROR); }
void Logger::Debug(const std::string msg) { Log(msg, LOGDEBUG); }
void Logger::Fatal(const std::string msg) {
  Log(msg, LOGFATAL);
#ifndef DEBUG
  exit(1);
#endif  // !DEBUG
}

void Logger::Log(const std::string msg, LogLevel level) {
  Log(msg.c_str(), level);
}

void Logger::Log(const char* msg, LogLevel level) {
  std::cout << "[" << toLevelString(level) << "]: " << msg << std::endl;
}

const char* Logger::toLevelString(LogLevel level) {
  switch (level) {
    case LOGINFO:
      return "Info";
    case LOGWARN:
      return "Warn";
    case LOGERROR:
      return "Error";
    case LOGDEBUG:
      return "Debug";
    case LOGFATAL:
      return "FatalError";
    default:
      return "Unknown";
  }
}