#include <iostream>
#include <cassert>

#include "PxeLogger.h"

#ifndef PXENGINESAMPLES_LOG_HANDLER_H_
#define PXENGINESAMPLES_LOG_HANDLER_H_

class LogHandle : public pxengine::PxeLogInterface {
public:
	void onLog(pxengine::PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line) override {
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
};

#endif // !PXENGINESAMPLES_LOG_HANDLER_H_
