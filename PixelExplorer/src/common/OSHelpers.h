#if (_WIN32 || _WIN64 || __CYGWIN__)
#define PX_WIN_OS
#elif (__linux__ || linux || __linux)
#define PX_LINUX_OS
#else
#error OSHelpers.h Unsupported OS, expected _WIN32 || _WIN64 || __CYGWIN__ || __linux__ || linux || __linux to be defined
#endif

#include <filesystem>
#include <stdint.h>

#ifdef PX_WIN_OS
#include <windows.h>
#endif // PX_WIN_OS

#include "Logger.h"

#ifndef PIXELEXPLORER_OSHELPERS_H_
#define PIXELEXPLORER_OSHELPERS_H_
namespace pixelexplorer::OSHelper {
	inline std::filesystem::path getExecutablePath() {
#if defined(PX_WIN_OS)
		uint32_t bufferSize = 512;
		char* pathBuf = nullptr;
		DWORD result;
		DWORD lastError;
		do {
			if (pathBuf != nullptr) free(pathBuf);
			bufferSize *= 2;
			pathBuf = (char*)calloc(bufferSize, sizeof(char));
			result = GetModuleFileNameA(nullptr, pathBuf, bufferSize);
			lastError = GetLastError();
		} while (result == 0 || result == bufferSize && (lastError == ERROR_INSUFFICIENT_BUFFER || lastError == ERROR_SUCCESS));
		std::filesystem::path exePath(pathBuf);
		free(pathBuf);
		return exePath;
#elif defined(PX_LINUX_OS)
		return std::filesystem::canonical("/proc/self/exe");
#endif
	}

	inline std::filesystem::path getExecutableDirectory() {
		return getExecutablePath().remove_filename();
	}

	inline std::filesystem::path getAssetPath() {
		std::filesystem::path assetPath(getExecutableDirectory() / "assets");
		std::error_code errorCode;
		std::filesystem::create_directories(assetPath, errorCode);
		if (errorCode.value() != 0) Logger::error(__FUNCTION__" failed to create asset path " + errorCode.message());
		return assetPath;
	}

	// this function assumes that subpath is only a directory
	inline std::filesystem::path getAssetPath(const std::filesystem::path& subpath) {
		std::filesystem::path assetPath(getExecutableDirectory() / "assets" / subpath);
		std::error_code errorCode;
		std::filesystem::create_directories(assetPath, errorCode);
		if (errorCode.value() != 0) Logger::error(__FUNCTION__" failed to create asset path " + errorCode.message());
		return assetPath;
	}
}
#endif // PIXELEXPLORER_OSHELPERS_H_
