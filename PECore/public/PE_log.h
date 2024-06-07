#ifndef PE_LOG_H_
#define PE_LOG_H_
#include "PE_defines.h"
#include "PE_memory.h"
#include "SDL_stdinc.h"

namespace pe::log {
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

	PE_EXTERN_C PE_API void PE_CALL PE_LogMessage(LogPriority log_priority, const char* msg, const char* file, const char* function, int line);

#define PE_LogFormattedMessage(log_priority, msg, ...) pe::log::LogFormattedMessage(log_priority, msg, PE_TEXT(__FUNCTION__), PE_TEXT(__FILE__), __LINE__, ##__VA_ARGS__)
#define PE_LogDebug(msg, ...) PE_LogFormattedMessage(pe::log::PE_LOG_DEBUG, msg, ##__VA_ARGS__)
#define PE_LogVerbose(msg, ...) PE_LogFormattedMessage(pe::log::PE_LOG_VERBOSE, msg, ##__VA_ARGS__)
#define PE_LogInfo(msg, ...) PE_LogFormattedMessage(pe::log::PE_LOG_INFO, msg, ##__VA_ARGS__)
#define PE_LogWarn(msg, ...) PE_LogFormattedMessage(pe::log::PE_LOG_WARN, msg, ##__VA_ARGS__)
#define PE_LogError(msg, ...) PE_LogFormattedMessage(pe::log::PE_LOG_ERROR, msg, ##__VA_ARGS__)
#define PE_LogCritical(msg, ...) PE_LogFormattedMessage(pe::log::PE_LOG_CRITICAL, msg, ##__VA_ARGS__)

	template<class... Args>
	void LogFormattedMessage(LogPriority log_priority, const char* msg, const char* file, const char* function, int line, Args... args) {
		char stack_buf[256];
		char* formatted_msg = msg;
		constexpr if (sizeof...(args) > 0) {
			int msg_length = SDL_snprintf(stack_buf, sizeof(stack_buf), msg);
			if (msg_length < 0) {
				PE_LogError(PE_TEXT("Failed to format log string, printing unformatted string instead"));
			}
			else if (msg_length >= sizeof(stack_buf)) {
				char* buf = PE_malloc(msg_length + 1);
				if (buf) {
					msg_length = SDL_snprintf(buf, sizeof(stack_buf), msg);
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
			PE_free(formatted_msg);
		}
	}

#define PE_Assert(condition, fmt, ...) do { if (!(condition)) { PE_LogCritical(fmt, __VA_ARGS__); } } while(false)
#if PE_DEBUG
#define PE_DebugAssert PE_Assert
#else
#define PE_DebugAssert ((void)0)
#endif
}
#endif // !PE_LOG_H_