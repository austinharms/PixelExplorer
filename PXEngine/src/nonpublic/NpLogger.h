#ifndef PXENGINE_NONPUBLIC_LOGGER_H_
#define PXENGINE_NONPUBLIC_LOGGER_H_
#include <string>

#include "PxeTypes.h"
#include "PxeLogger.h"
#include "NpEngine.h"

// TODO Add config switches to disable logging features
#define PXE_INFO(msg) pxengine::nonpublic::log(pxengine::PxeLogLevel::PXE_INFO, msg, __FILE__, __FUNCTION__, __LINE__)
#define PXE_WARN(msg) pxengine::nonpublic::log(pxengine::PxeLogLevel::PXE_WARN, msg, __FILE__, __FUNCTION__, __LINE__)
#define PXE_ERROR(msg) pxengine::nonpublic::log(pxengine::PxeLogLevel::PXE_ERROR, msg, __FILE__, __FUNCTION__, __LINE__)
#define PXE_FATAL(msg) pxengine::nonpublic::log(pxengine::PxeLogLevel::PXE_FATAL, msg, __FILE__, __FUNCTION__, __LINE__)
#define PXE_ASSERT(exp, msg) ((!!(exp))?((void)0):pxengine::pxeGetAssertInterface().onAssert(msg, __FILE__, __FUNCTION__, __LINE__))
#define PXE_CHECKSDLERROR() do { const char* error = SDL_GetError(); if (*error != '\0') { PXE_ERROR(error); SDL_ClearError(); } } while (0)

namespace pxengine {
	namespace nonpublic {
		inline void log(PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line) {
			NpEngine::getInstance().getLogInterface().onLog(level, msg, file, function, line);
		}

		inline void log(PxeLogLevel level, const std::string& msg, const char* file, const char* function, uint64_t line) { 
			log(level, msg.c_str(), file, function, line); 
		}
	}
}
#endif // !PXENGINE_NONPUBLIC_LOGGER_H_
