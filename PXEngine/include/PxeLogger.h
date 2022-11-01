#include <stdint.h>

#ifndef PXENGINE_LOGGER_H_
#define PXENGINE_LOGGER_H_
namespace pxengine {
	enum class LogLevel : uint8_t
	{
		INFO = 0,
		WARN,
		ERROR,
		FATAL,
		LOGLEVELCOUNT
	};

	class PxeAssertHandler
	{
	public:
		virtual ~PxeAssertHandler() {}
		virtual void onAssert(const char* msg, const char* file, uint64_t line, const char* function) = 0;
	};

	class PxeLogHandler
	{
	public:
		virtual ~PxeLogHandler() {}
		virtual void onLog(const char* msg, uint8_t level, const char* file, uint64_t line, const char* function) = 0;
	};

	// get the PxeAssertHandler used when an assertion is made
	PxeAssertHandler& getPXEAssertHandler();

	// set the PxeAssertHandler used when an assertion is made
	void setPXEAssertHandler(PxeAssertHandler& assertHandler);
}
#endif // !PXENGINE_LOGGER_H_
