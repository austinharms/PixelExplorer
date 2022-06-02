#include <iostream>
#include <assert.h>

namespace pixelexplorer::Logger {
	const char* levelString(LogLevel level) {
		switch (level)
		{
		case LogLevel::DEBUG:
			return "DEBUG";

		case LogLevel::INFO:
			return "INFO";

		case LogLevel::WARN:
			return "WARN";

		case LogLevel::ERROR:
			return "ERROR";

		case LogLevel::FATAL:
			return "FATAL";

		default:
			// Should not happen
			assert(false);
			return "UNKNOWN";
		}
	}

	void debug(const std::string& msg) {
		log(msg, LogLevel::DEBUG);
	}

	void info(const std::string& msg) {
		log(msg, LogLevel::INFO);
	}

	void warn(const std::string& msg) {
		log(msg, LogLevel::WARN);
	}

	void error(const std::string& msg) {
		log(msg, LogLevel::ERROR);
	}

	void fatal(const std::string& msg) {
		log(msg, LogLevel::FATAL);
	}

	void log(const std::string& msg, LogLevel level) {
		log(msg.c_str(), level);
	}

	void debug(const char* msg) {
		log(msg, LogLevel::DEBUG);
	}

	void info(const char* msg) {
		log(msg, LogLevel::INFO);
	}

	void warn(const char* msg) {
		log(msg, LogLevel::WARN);
	}

	void error(const char* msg) {
		log(msg, LogLevel::ERROR);
	}

	void fatal(const char* msg) {
		log(msg, LogLevel::FATAL);
	}

	void log(const char* msg, LogLevel level) {
		std::cout << "[" << levelString(level) << "] " << msg << std::endl;
#ifdef DEBUG
		assert(level != LogLevel::FATAL);
#else
		if (level == LogLevel::FATAL) exit(-1);
#endif
	}
}