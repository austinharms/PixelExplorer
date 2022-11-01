#include "NpLogger.h"

#include "NpEngineBase.h"

namespace pxengine::nonpublic {
	void log(const std::string& msg, LogLevel level, const char* file, uint64_t line, const char* function)
	{
		log(msg.c_str(), level, file, line, function);
	}

	void log(const char* msg, LogLevel level, const char* file, uint64_t line, const char* function)
	{
		NpEngineBase::getInstance().onLog(msg, (uint8_t)level, file, line, function);
	}
}
