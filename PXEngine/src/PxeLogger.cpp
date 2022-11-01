#include "PxeLogger.h"

#include <stdlib.h>
#include <iostream>

namespace {
	class PxeDefaultAssertHandler : public pxengine::PxeAssertHandler
	{
	public:
		virtual ~PxeDefaultAssertHandler() {}
		void onAssert(const char* msg, const char* file, uint64_t line, const char* function) override {
			std::cout << "PXENGINE ASSERT!: " << msg << " File: " << file << " Function: " << function << " Line: " << line << std::endl;
			abort();
		}
	};

	PxeDefaultAssertHandler defaultAssertHandler;
	pxengine::PxeAssertHandler* pxeAssertHandler = &defaultAssertHandler;
}

namespace pxengine {
	PxeAssertHandler& getPXEAssertHandler()
	{
		return *pxeAssertHandler;
	}

	void setPXEAssertHandler(PxeAssertHandler& assertHandler)
	{
		pxeAssertHandler = &assertHandler;
	}
}