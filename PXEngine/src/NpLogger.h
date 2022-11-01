#include <stdint.h>
#include <string>

#include "PxeLogger.h"

#ifndef PXENGINE_NONPUBLIC_LOGGER_H_
#define PXENGINE_NONPUBLIC_LOGGER_H_

#define PXE_INFO(msg) pxengine::nonpublic::log(msg, pxengine::LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__)
#define PXE_WARN(msg) pxengine::nonpublic::log(msg, pxengine::LogLevel::WARN, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ERROR(msg) pxengine::nonpublic::log(msg, pxengine::LogLevel::ERROR, __FILE__, __LINE__, __FUNCTION__)
#define PXE_FATAL(msg) pxengine::nonpublic::log(msg, pxengine::LogLevel::FATAL, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ASSERT(exp, msg) ((!!(exp))?((void)0):pxengine::getPXEAssertHandler().onAssert(msg, __FILE__, __LINE__, __FUNCTION__))

namespace pxengine::nonpublic {
	void log(const std::string& msg, LogLevel level, const char* file, uint64_t line, const char* function);
	void log(const char* msg, LogLevel level, const char* file, uint64_t line, const char* function);
}
#endif // !PXENGINE_NONPUBLIC_LOGGER_H_
