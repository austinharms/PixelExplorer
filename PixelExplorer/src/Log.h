#ifndef PIXELEXPLORER_LOG_H_
#define PIXELEXPLORER_LOG_H_
#include "PxeTypes.h"
#include "PxeLogger.h"
#include "PxeOSHelpers.h"

#define PEX_INFO(msg) pixelexplorer::Log::log(pxengine::PxeLogLevel::PXE_INFO, msg, __FILE__, __FUNCTION__, __LINE__);
#define PEX_WARN(msg) pixelexplorer::Log::log(pxengine::PxeLogLevel::PXE_WARN, msg, __FILE__, __FUNCTION__, __LINE__);
#define PEX_ERROR(msg) pixelexplorer::Log::log(pxengine::PxeLogLevel::PXE_ERROR, msg, __FILE__, __FUNCTION__, __LINE__);
#define PEX_FATAL(msg) pixelexplorer::Log::log(pxengine::PxeLogLevel::PXE_FATAL, msg, __FILE__, __FUNCTION__, __LINE__);

namespace pixelexplorer {
	class Log : public pxengine::PxeLogInterface
	{
	public:
		static void log(pxengine::PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line);
		void onLog(pxengine::PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line) override;
		Log();
		~Log();

	private:
#ifdef PXE_WIN_OS
		void* _stdOutHandle;
#endif // PXE_WIN_OS

	};
}
#endif // !PIXELEXPLORER_LOG_H_
