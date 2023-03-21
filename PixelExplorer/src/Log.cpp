#include "Log.h"

#include "PxeEngine.h"
#include <iostream>

namespace pixelexplorer {
	void Log::log(pxengine::PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line)
	{
		pxengine::PxeEngine::getInstance().getLoggingInterface().onLog(level, msg, file, function, line);
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