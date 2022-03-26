#include "Logger.h"
namespace px {

void Logger::info(const char* msg) { log(msg, LOGINFO); }
void Logger::warn(const char* msg) { log(msg, LOGWARN); }
void Logger::error(const char* msg) { log(msg, LOGERROR); }
void Logger::debug(const char* msg) { log(msg, LOGDEBUG); }
void Logger::fatal(const char* msg) {
  log(msg, LOGFATAL);
#ifndef DEBUG
  exit(1);
#endif  // !DEBUG
}

void Logger::info(const std::string msg) { log(msg, LOGINFO); }
void Logger::warn(const std::string msg) { log(msg, LOGWARN); }
void Logger::error(const std::string msg) { log(msg, LOGERROR); }
void Logger::debug(const std::string msg) { log(msg, LOGDEBUG); }
void Logger::fatal(const std::string msg) {
  log(msg, LOGFATAL);
#ifndef DEBUG
  exit(1);
#endif  // !DEBUG
}

void Logger::log(const std::string msg, LogLevel level) {
  log(msg.c_str(), level);
}

void Logger::log(const char* msg, LogLevel level) {
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
}  // namespace px