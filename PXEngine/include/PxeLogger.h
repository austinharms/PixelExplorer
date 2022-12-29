#include <stdint.h>

#ifndef PXENGINE_LOGGER_H_
#define PXENGINE_LOGGER_H_
namespace pxengine {
	enum class PxeLogLevel : uint8_t
	{
		INFO = 0,
		WARN,
		ERROR,
		FATAL,
		LOGLEVELCOUNT
	};

	class PxeAssertInterface
	{
	public:
		virtual void onAssert(const char* msg, const char* file, const char* function, uint64_t line) = 0;
	};

	class PxeLogInterface
	{
	public:
		virtual void onLog(PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line) = 0;
	};

	// get the PxeAssertInterface used when an assertion is made
	extern PxeAssertInterface& pxeGetAssertInterface();

	// set the PxeAssertInterface used when an assertion is made
	// note log FATAL is preferred to an assertion
	// assertion will only happen if there was an error and the PxeLogInterface is not available 
	extern void pxeSetAssertInterface(PxeAssertInterface& assertInterface);
}
#endif // !PXENGINE_LOGGER_H_
