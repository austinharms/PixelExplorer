// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#ifndef PE_LOG_H_
#define PE_LOG_H_
#include "PE_defines.h"
#include "PE_memory.h"
#include "SDL3/SDL_stdinc.h"

namespace pe {
	enum LogPriority
	{
		PE_LOG_DEBUG = 0,
		PE_LOG_VERBOSE,
		PE_LOG_INFO,
		PE_LOG_WARN,
		PE_LOG_ERROR,
		PE_LOG_CRITICAL,
		PE_LOG_ENUM_COUNT
	};

	PE_EXTERN_C PE_API void PE_CALL PE_LogMessage(LogPriority log_priority, const char8_t* msg, const char8_t* file, const char8_t* function, int line) PE_NOEXCEPT;

#define PE_LogFormattedMessage(log_priority, msg, ...) pe::LogFormattedMessage(log_priority, msg, PE_FILE_NAME, PE_FUNCTION_NAME, PE_FILE_LINE, ##__VA_ARGS__)
#define PE_LogDebug(msg, ...) PE_LogFormattedMessage(pe::PE_LOG_DEBUG, msg, ##__VA_ARGS__)
#define PE_LogVerbose(msg, ...) PE_LogFormattedMessage(pe::PE_LOG_VERBOSE, msg, ##__VA_ARGS__)
#define PE_LogInfo(msg, ...) PE_LogFormattedMessage(pe::PE_LOG_INFO, msg, ##__VA_ARGS__)
#define PE_LogWarn(msg, ...) PE_LogFormattedMessage(pe::PE_LOG_WARN, msg, ##__VA_ARGS__)
#define PE_LogError(msg, ...) PE_LogFormattedMessage(pe::PE_LOG_ERROR, msg, ##__VA_ARGS__)
#define PE_LogCritical(msg, ...) PE_LogFormattedMessage(pe::PE_LOG_CRITICAL, msg, ##__VA_ARGS__)
#define PE_Assert(condition, msg, ...) do { if (!(condition)) { PE_LogCritical(msg, ##__VA_ARGS__); } } while(false)
#if PE_DEBUG
#define PE_DebugAssert PE_Assert
#else
#define PE_DebugAssert(...) ((void)0)
#endif

	template<class... Args>
	void LogFormattedMessage(LogPriority log_priority, const char8_t* msg, const char8_t* file, const char8_t* function, int line, Args... args) {
		char8_t stack_buf[256];
		const char8_t* formatted_msg = msg;
		 if constexpr (sizeof...(args) > 0) {
			int msg_length = SDL_snprintf(reinterpret_cast<char*>(stack_buf), sizeof(stack_buf), reinterpret_cast<const char*>(msg), args...);
			if (msg_length < 0) {
				PE_LogError(PE_TEXT("Failed to format log string, printing unformatted string instead"));
			}
			else if (msg_length >= sizeof(stack_buf)) {
				PE_STATIC_ASSERT(sizeof(char8_t) == 1, PE_TEXT("Expected char8_t to be 1 byte in size"));
				char8_t* buf = static_cast<char8_t*>(PE_malloc(msg_length + 1));
				if (buf) {
					msg_length = SDL_snprintf(reinterpret_cast<char*>(buf), sizeof(stack_buf), reinterpret_cast<const char*>(msg), args...);
					if (msg_length >= 0) {
						formatted_msg = buf;
					}
					else {
						PE_LogError(PE_TEXT("Failed to format log string, printing unformatted string instead"));
						PE_free(buf);
					}
				}
				else {
					PE_LogError(PE_TEXT("Failed to allocate log string, printing unformatted string instead"));
				}
			}
			else {
				formatted_msg = stack_buf;
			}
		}

		PE_LogMessage(log_priority, formatted_msg, file, function, line);
		if (formatted_msg != stack_buf && formatted_msg != msg) {
			PE_free(const_cast<char8_t*>(formatted_msg));
		}
	}

}
#endif // !PE_LOG_H_
