#include "Log.h"
#include <iostream>

namespace pixelexplorer {
	Log& Log::getInstance()
	{
		static Log log;
		return log;
	}

	void Log::info(const char* msg, const char* file, const char* function, uint64_t line)
	{
		getInstance().onLog(pxengine::PxeLogLevel::PXE_INFO, msg, file, function, line);
	}

	void Log::warn(const char* msg, const char* file, const char* function, uint64_t line)
	{
		getInstance().onLog(pxengine::PxeLogLevel::PXE_WARN, msg, file, function, line);
	}

	void Log::error(const char* msg, const char* file, const char* function, uint64_t line)
	{
		getInstance().onLog(pxengine::PxeLogLevel::PXE_ERROR, msg, file, function, line);
	}

	void Log::fatal(const char* msg, const char* file, const char* function, uint64_t line)
	{
		getInstance().onLog(pxengine::PxeLogLevel::PXE_FATAL, msg, file, function, line);
	}

	void Log::onLog(pxengine::PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line)
	{
		switch (level)
		{
		case pxengine::PxeLogLevel::PXE_INFO:
			std::cout << "[INFO] ";
			break;
		case pxengine::PxeLogLevel::PXE_WARN:
			std::cout << "[WARN] ";
			break;
		case pxengine::PxeLogLevel::PXE_ERROR:
			std::cout << "[ERROR] ";
			break;
		case pxengine::PxeLogLevel::PXE_FATAL:
			std::cout << "[FATAL] ";
			break;
		}

		//std::cout << "Log: " << msg << " Level: " << level << " File: " << file << " Function: " << function << " Line: " << line << std::endl;
		std::cout << msg << std::endl;
		assert(level != pxengine::PxeLogLevel::PXE_FATAL);
	}
}