#ifndef PXENGINE_LOGGER_H_
#define PXENGINE_LOGGER_H_
PXE_PRIVATE_IMPLEMENTATION_START
#include <string>

// TODO Add config switches to disable logging features
#define PXE_INFO(msg) pxengine::pxeLog(pxengine::PxeLogLevel::PXE_INFO, msg, __FILE__, __FUNCTION__, __LINE__)
#define PXE_WARN(msg) pxengine::pxeLog(pxengine::PxeLogLevel::PXE_WARN, msg, __FILE__, __FUNCTION__, __LINE__)
#define PXE_ERROR(msg) pxengine::pxeLog(pxengine::PxeLogLevel::PXE_ERROR, msg, __FILE__, __FUNCTION__, __LINE__)
#define PXE_FATAL(msg) pxengine::pxeLog(pxengine::PxeLogLevel::PXE_FATAL, msg, __FILE__, __FUNCTION__, __LINE__)
#define PXE_ASSERT(exp, msg) ((!!(exp))?((void)0):pxengine::pxeGetAssertInterface().onAssert(msg, __FILE__, __FUNCTION__, __LINE__))
#define PXE_CHECKSDLERROR() do { const char* error = SDL_GetError(); if (*error != '\0') { PXE_ERROR(error); SDL_ClearError(); } } while (0)
PXE_PRIVATE_IMPLEMENTATION_END
#include "PxeTypes.h"

namespace pxengine {
	class PxeAssertInterface
	{
	public:
		// Called when an assertion is made by the PxeEngine
		virtual void onAssert(const char* msg, const char* file, const char* function, uint64_t line) = 0;
	};

	class PxeLogInterface
	{
	public:
		// Called when the PxeEngine output log information/errors
		virtual void onLog(PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line) = 0;
	};

	// Returns the PxeAssertInterface used when an assertion is made
	// Note: log FATAL is preferred to an assertion, an assertion will only happen if there was a unrecoverable error and the PxeLogInterface is not available
	extern PxeAssertInterface& pxeGetAssertInterface();

	// Set the PxeAssertInterface used when an assertion is made
	// Note: log FATAL is preferred to an assertion, an assertion will only happen if there was a unrecoverable error and the PxeLogInterface is not available
	extern void pxeSetAssertInterface(PxeAssertInterface& assertInterface);
	PXE_PRIVATE_IMPLEMENTATION_START
	extern void pxeLog(PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line);
	inline void pxeLog(PxeLogLevel level, const std::string& msg, const char* file, const char* function, uint64_t line) {
		pxeLog(level, msg.c_str(), file, function, line);
	}
	PXE_PRIVATE_IMPLEMENTATION_END
}
#endif // !PXENGINE_LOGGER_H_
