#include "NpLogger.h"

#include "NpEngineBase.h"

namespace pxengine {
	namespace nonpublic {
		namespace {
			PxeLogInterface* pxeLogInterface = nullptr;
		}

		void setLogInterface(PxeLogInterface& logInterface) {
			pxeLogInterface = &logInterface;
		}

		void clearLogInterface() {
			pxeLogInterface = nullptr;
		}

		void log(PxeLogLevel level, const std::string& msg, const char* file, const char* function, uint64_t line)
		{
			log(level, msg.c_str(), file, function, line);
		}

		void log(PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line)
		{
#ifdef PXE_DEBUG
			PXE_ASSERT(pxeLogInterface, "Attempted to log with no PxeLogInterface set");
#else
			if (!pxeLogInterface) return;
#endif // PXE_DEBUG
			pxeLogInterface->onLog(level, msg, file, function, line);
		}
	}
}
