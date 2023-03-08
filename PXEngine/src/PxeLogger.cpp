#include "PxeLogger.h"

#ifndef PXE_DISABLE_DEFAULT_ASSERT_LOG
#include <iostream>
#endif // !PXE_DISABLE_DEFAULT_ASSERT_LOG
#ifndef PXE_DISABLE_DEFAULT_ASSERT_ABORT
#include <stdlib.h>
#endif // !PXE_DISABLE_DEFAULT_ASSERT_ABORT

#include "PxeEngine.h"

namespace pxengine {
	namespace {
		class PxeDefaultAssertHandler : public pxengine::PxeAssertInterface
		{
		public:
			void onAssert(const char* msg, const char* file, const char* function, uint64_t line) override {
#ifndef PXE_DISABLE_DEFAULT_ASSERT_LOG
				std::cout << "PXENGINE ASSERT!: \"" << msg << "\" File: " << file << " Function: " << function << " Line: " << line << std::endl;
#endif // !PXE_DISABLE_DEFAULT_ASSERT_LOG
#ifndef PXE_DISABLE_DEFAULT_ASSERT_ABORT
				abort();
#endif // !PXE_DISABLE_DEFAULT_ASSERT_ABORT
			}
		};

		PxeDefaultAssertHandler defaultAssertHandler;
		pxengine::PxeAssertInterface* pxeAssertInterface = &defaultAssertHandler;
	}

	PXE_NODISCARD PxeAssertInterface& pxeGetAssertInterface()
	{
		return *pxeAssertInterface;
	}

	void pxeSetAssertInterface(PxeAssertInterface& assertInterface)
	{
		pxeAssertInterface = &assertInterface;
	}

	void pxeLog(PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line)
	{
		PxeEngine::getInstance().getLoggingInterface().onLog(level, msg, file, function, line);
	}
}