#include "PxeFSHelpers.h"

// ################ WARNING all of the PXE_LINUX_OS code in this file is untested ################

#if (_WIN32 || _WIN64 || __CYGWIN__)
#define PXE_WIN_OS
#elif (__linux__ || linux || __linux)
#define PXE_LINUX_OS
#else
#error PxeFSHelpers Unsupported OS, expected _WIN32 || _WIN64 || __CYGWIN__ || __linux__ || linux || __linux to be defined
#endif

#include <stdint.h>
#include <filesystem>

#ifdef PXE_WIN_OS
#include <windows.h>
#include <shlobj.h>
#endif // PXE_WIN_OS

#ifdef PXE_LINUX_OS
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif // PXE_LINUX_OS

#include "NpLogger.h"

namespace pxengine {
	PXE_NODISCARD std::filesystem::path getExecutablePath()
	{
#if defined(PXE_WIN_OS)
		uint32_t bufferSize = 128;
		char* pathBuf = nullptr;
		DWORD result;
		DWORD lastError;
		do {
			if (pathBuf != nullptr) free(pathBuf);
			bufferSize *= 2;
			pathBuf = (char*)calloc(bufferSize, sizeof(char));
			if (pathBuf == nullptr) {
				PXE_ERROR("Failed to allocate executable path buffer");
				bufferSize /= 2;
				continue;
			}

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

	PXE_NODISCARD std::filesystem::path getExecutableDirectory()
	{
		return getExecutablePath().remove_filename();
	}

	PXE_NODISCARD std::filesystem::path getAssetPath(const std::filesystem::path& subpath)
	{
		std::filesystem::path assetPath(getExecutableDirectory() / "assets" / subpath);
		std::error_code errorCode;
		std::filesystem::create_directories(assetPath, errorCode);
#ifdef PXE_DEBUG
		if (errorCode.value()) {
			PXE_ERROR("Failed to create asset path " + errorCode.message());
		}
#endif // PXE_DEBUG
		return assetPath;
	}

	PXE_NODISCARD std::filesystem::path getPersistentPath(const std::filesystem::path& subpath)
	{
		std::filesystem::path userPath;

#if defined(PXE_WIN_OS)
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

#elif defined(PXE_LINUX_OS)
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
		userPath /= subpath;
		// TODO Add check for file write permissions
		std::error_code errorCode;
		std::filesystem::create_directories(userPath, errorCode);
		if (errorCode.value()) {
			PXE_ERROR("Failed to create persistent path " + errorCode.message());
		}

		return userPath;
	}
}

#ifdef PXE_WIN_OS
#undef PXE_WIN_OS
#endif // PXE_WIN_OS

#ifdef PXE_LINUX_OS
#undef PXE_LINUX_OS
#endif // PXE_LINUX_OS

