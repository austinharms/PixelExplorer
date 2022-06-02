#include <string>

#ifndef PIXELEXPLORER_LOGGER_H_
#define PIXELEXPLORER_LOGGER_H_
namespace pixelexplorer::Logger {
	enum class LogLevel
	{
		DEBUG = 0,
		INFO,
		WARN,
		ERROR,
		FATAL
	};

	inline const char* levelString(LogLevel level);
	inline void debug(const std::string& msg);
	inline void info(const std::string& msg);
	inline void warn(const std::string& msg);
	inline void error(const std::string& msg);
	inline void fatal(const std::string& msg);
	inline void log(const std::string& msg, LogLevel level);
	inline void debug(const char* msg);
	inline void info(const char* msg);
	inline void warn(const char* msg);
	inline void error(const char* msg);
	inline void fatal(const char* msg);
	inline void log(const char* msg, LogLevel level);
}

#include "Logger.inl"
#endif // !PIXELEXPLORER_LOGGER_H_

