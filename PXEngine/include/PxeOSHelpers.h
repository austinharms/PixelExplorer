#ifndef PXENGINE_OS_HELPERS_H_
#define PXENGINE_OS_HELPERS_H_
#if (_WIN32 || _WIN64 || __CYGWIN__)
#define PXE_WIN_OS
#elif (__linux__ || linux || __linux)
#define PXE_LINUX_OS
#else
#error PxeOSHelper Unsupported OS, expected _WIN32 || _WIN64 || __CYGWIN__ || __linux__ || linux || __linux to be defined
#endif

#include "PxeTypes.h"

namespace pxengine {
	// Sets the calling threads name, returns true on successes
	// Note: this currently only works for windows 10 and up
	bool setThreadName(const wchar_t* threadName);
}
#endif // !PXENGINE_OS_HELPERS_H_
