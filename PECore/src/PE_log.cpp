#include "PE_log.h"
#include <cstdlib>
#include <iostream>

namespace pe::log {
	PE_STATIC_ASSERT(PE_LOG_ENUM_COUNT == 6, PE_TEXT("log_enum_lookup_table invalid"));
	constexpr const char* log_enum_lookup_table[PE_LOG_ENUM_COUNT] = {
		"[DEBUG]",
		"[VERBOSE]",
		"[INFO]",
		"[WARN]",
		"[ERROR]",
		"[CRITICAL]",
	};

	void PE_LogMessage(LogPriority log_priority, const char* msg, const char* file, const char* function, int line) {
		if (log_priority >= PE_LOG_ENUM_COUNT) {
			log_priority = PE_LOG_CRITICAL;
		}

		std::cout  << log_enum_lookup_table[log_priority] << msg << std::endl;
		if (log_priority == PE_LOG_CRITICAL) {
			std::abort();
		}
	}
}
