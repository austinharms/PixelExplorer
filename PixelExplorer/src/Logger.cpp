#include "Logger.h"

inline void Logger::Info(const char* msg) { Log(msg, LOGINFO); }
inline void Logger::Warn(const char* msg) { Log(msg, LOGWARN); }
inline void Logger::Error(const char* msg) { Log(msg, LOGERROR); }
inline void Logger::Debug(const char* msg) { Log(msg, LOGDEBUG); }
inline void Logger::Fatal(const char* msg) {
  Log(msg, LOGFATAL);
#ifndef DEBUG
  exit(1);
#endif  // !DEBUG
}

inline void Logger::Info(const std::string msg) { Log(msg, LOGINFO); }
inline void Logger::Warn(const std::string msg) { Log(msg, LOGWARN); }
inline void Logger::Error(const std::string msg) { Log(msg, LOGERROR); }
inline void Logger::Debug(const std::string msg) { Log(msg, LOGDEBUG); }
inline void Logger::Fatal(const std::string msg) {
  Log(msg, LOGFATAL);
#ifndef DEBUG
  exit(1);
#endif  // !DEBUG
}

inline void Logger::Log(const std::string msg, LogLevel level) {
  Log(msg.c_str(), level);
}

inline void Logger::Log(const char* msg, LogLevel level) {
  std::cout << "[" << toLevelString(level) << "]: " << msg << std::endl;
}

inline const char* Logger::toLevelString(LogLevel level) {
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