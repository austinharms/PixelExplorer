#include "PxeOSHelpers.h"

#ifdef PXE_WIN_OS
#include <windows.h>
#include <processthreadsapi.h>
#endif // PXE_WIN_OS

#include "PxeLogger.h"

namespace pxengine {
	bool setThreadName(const wchar_t* threadName)
	{
#ifdef PXE_WIN_OS
		HRESULT hr = SetThreadDescription(GetCurrentThread(), threadName);
		return !FAILED(hr);
#else
		PXE_WARN("setThreadName is unsupported with this OS");
		return false;
#endif // PXE_WIN_OS

	}
}
