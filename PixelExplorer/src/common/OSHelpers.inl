#if (_WIN32 || _WIN64 || __CYGWIN__)
#define PX_WIN_OS
#elif (__linux__ || linux || __linux)
#define PX_LINUX_OS
#else
#error OSHelpers.h Unsupported OS, expected _WIN32 || _WIN64 || __CYGWIN__ || __linux__ || linux || __linux to be defined
#endif

#include <stdint.h>
#include <filesystem>

#ifdef PX_WIN_OS
#include <windows.h>
#include <shlobj.h>
#endif // PX_WIN_OS

#ifdef PX_LINUX_OS
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#include "Logger.h"

// WARNING all of the PX_LINUX_OS code in this file is untested
namespace pixelexplorer::OSHelper {
	std::filesystem::path getExecutablePath() {
#if defined(PX_WIN_OS)
		uint32_t bufferSize = 512;
		char* pathBuf = nullptr;
		DWORD result;
		DWORD lastError;
		do {
			if (pathBuf != nullptr) free(pathBuf);
			bufferSize *= 2;
			pathBuf = (char*)calloc(bufferSize, sizeof(char));
			if (pathBuf == nullptr) Logger::fatal(__FUNCTION__" failed to allocate executable path buffer");
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

	std::filesystem::path getExecutableDirectory() {
		return getExecutablePath().remove_filename();
	}

	std::filesystem::path getAssetPath(const std::filesystem::path& subpath) {
		std::filesystem::path assetPath(getExecutableDirectory() / "assets" / subpath);
		std::error_code errorCode;
		std::filesystem::create_directories(assetPath, errorCode);
#ifdef DEBUG
		// Nice to know in debug but, we don't guarantee folder creation and should not generate errors if it fails
		if (errorCode.value() != 0) Logger::error(__FUNCTION__" failed to create asset path " + errorCode.message());
#endif // DEBUG
		return assetPath;
	}

	std::filesystem::path getPersistentPath(const std::filesystem::path& subpath) {
		std::filesystem::path userPath;

#if defined(PX_WIN_OS)
		PWSTR rawUserPath;
		HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &rawUserPath);
		if (result == S_OK) {
			userPath = rawUserPath;
		}
		else {
			// not a great option, but use it as a fall back if SHGetKnownFolderPath fails
			userPath = std::filesystem::temp_directory_path().parent_path().parent_path();
			userPath /= "Roaming";
		}

		CoTaskMemFree(rawUserPath);

#elif defined(PX_LINUX_OS)
		const char* rawUserPath;
		if ((rawUserPath = getenv("XDG_CONFIG_HOME"))) {
			userPath = rawUserPath;
		}
		else if ((rawUserPath = getenv("HOME"))) {
			userPath = rawUserPath;
		}
		else {
			userPath = getpwuid(getuid())->pw_dir;
		}
#endif

		userPath /= "PX_DATA";
		// should add check for file write permissions
		std::error_code errorCode;
		std::filesystem::create_directories(userPath, errorCode);
		if (errorCode.value() != 0) Logger::error(__FUNCTION__" failed to create persistent path " + errorCode.message());
		return "";
	}
}