#include "Log.h"

#include <iostream>

#include "PxeEngine.h"

/*
	Color Table
	00: Black
	01: Blue
	02: Green
	03: Aqua
	04: Red
	05: Purple
	06: Yellow
	07: White
	08: Gray
	09: Light_Blue
	10: Light_Green
	11: Light_Aqua
	12: Light_Red
	13: Light_Purple
	14: Light_Yellow
	15: Bright_White
*/

namespace pixelexplorer {
	Log::Log()
	{
#ifdef PXE_WIN_OS
		_stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(_stdOutHandle, 7);
#endif // PXE_WIN_OS
	}

	Log::~Log()
	{
#ifdef PXE_WIN_OS
		SetConsoleTextAttribute(_stdOutHandle, 7);
#endif // PXE_WIN_OS
	}

	void Log::log(pxengine::PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line)
	{
		pxengine::PxeEngine::getInstance().getLoggingInterface().onLog(level, msg, file, function, line);
	}

	void Log::onLog(pxengine::PxeLogLevel level, const char* msg, const char* file, const char* function, uint64_t line)
	{
		switch (level)
		{
		case pxengine::PxeLogLevel::PXE_INFO:
#ifdef PXE_WIN_OS
			SetConsoleTextAttribute(_stdOutHandle, 7);
#endif // PXE_WIN_OS
			std::cout << "[INFO] ";
			break;
		case pxengine::PxeLogLevel::PXE_WARN:
#ifdef PXE_WIN_OS
			SetConsoleTextAttribute(_stdOutHandle, 14);
#endif // PXE_WIN_OS
			std::cout << "[WARN] ";
			break;
		case pxengine::PxeLogLevel::PXE_ERROR:
#ifdef PXE_WIN_OS
			SetConsoleTextAttribute(_stdOutHandle, 12);
#endif // PXE_WIN_OS
			std::cout << "[ERROR] ";
			break;
		case pxengine::PxeLogLevel::PXE_FATAL:
#ifdef PXE_WIN_OS
			SetConsoleTextAttribute(_stdOutHandle, 4);
#endif // PXE_WIN_OS
			std::cout << "[FATAL] ";
			break;
		}

		//std::cout << "Log: " << msg << " Level: " << level << " File: " << file << " Function: " << function << " Line: " << line << std::endl;
		std::cout << msg << std::endl;
		assert(level != pxengine::PxeLogLevel::PXE_FATAL);
	}
}