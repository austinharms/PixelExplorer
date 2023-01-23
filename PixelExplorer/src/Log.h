#ifndef PIXELEXPLORER_LOG_H_
#define PIXELEXPLORER_LOG_H_
#include "PxeTypes.h"
#include "PxeLogger.h"

#define PEX_INFO(msg) pixelexplorer::Log::info(msg, __FILE__, __FUNCTION__, __LINE__);
#define PEX_WARN(msg) pixelexplorer::Log::warn(msg, __FILE__, __FUNCTION__, __LINE__);
#define PEX_ERROR(msg) pixelexplorer::Log::error(msg, __FILE__, __FUNCTION__, __LINE__);
#define PEX_FATAL(msg) pixelexplorer::Log::fatal(msg, __FILE__, __FUNCTION__, __LINE__);

namespace pixelexplorer {
	class Log : public pxengine::PxeLogInterface
	{
	public:
		static Log& getInstance();
		static void info(const char* msg, const char* file, const char* function, uint64_t line);
		static void warn(const char* msg, const char* file, const char* function, uint64_t line);
		static void error(const char* msg, const char* file, const char* function, uint64_t line);
		static void fatal(const char* msg, const char* file, const char* function, uint64_t line);

		void onLog(pxengine::PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line) override;
		~Log() = default;
	private:
		Log() = default;
	};
}
#endif // !PIXELEXPLORER_LOG_H_
