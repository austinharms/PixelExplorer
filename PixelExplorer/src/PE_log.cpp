// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#include "PE_log.h"
#include <cstdlib>
#include <iostream>

namespace pe {
	PE_STATIC_ASSERT(PE_LOG_ENUM_COUNT == 6, PE_TEXT("log_enum_lookup_table invalid"));
	constexpr const char8_t* log_enum_lookup_table[PE_LOG_ENUM_COUNT] = {
		PE_TEXT("[DEBUG]"),
		PE_TEXT("[VERBOSE]"),
		PE_TEXT("[INFO]"),
		PE_TEXT("[WARN]"),
		PE_TEXT("[ERROR]"),
		PE_TEXT("[CRITICAL]"),
	};

	void PE_LogMessage(LogPriority log_priority, const char8_t* msg, const char8_t* file, const char8_t* function, int line) PE_NOEXCEPT {
		if (log_priority >= PE_LOG_ENUM_COUNT) {
			log_priority = PE_LOG_CRITICAL;
		}

		std::cout << reinterpret_cast<const char*>(log_enum_lookup_table[log_priority]) << reinterpret_cast<const char*>(msg) << std::endl;
		if (log_priority == PE_LOG_CRITICAL) {
			std::abort();
		}
	}
}
