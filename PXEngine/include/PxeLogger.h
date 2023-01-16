#ifndef PXENGINE_LOGGER_H_
#define PXENGINE_LOGGER_H_
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
}
#endif // !PXENGINE_LOGGER_H_
