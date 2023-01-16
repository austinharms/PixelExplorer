#ifndef PXENGINE_FILESYSTEM_HELPERS_H_
#define PXENGINE_FILESYSTEM_HELPERS_H_
#include <filesystem>

#include "PxeTypes.h"

namespace pxengine {
	// Returns a path to the executable
	extern PXE_NODISCARD std::filesystem::path getExecutablePath();

	// Returns a path to the directory containing the executable
	extern PXE_NODISCARD std::filesystem::path getExecutableDirectory();

	// Returns a path to the application asset folder
	// Note: this folder may not be writable, if it is writable this also ensures subpath exist
	// Note: this function also assumes that subpath is only a directory or empty
	extern PXE_NODISCARD std::filesystem::path getAssetPath(const std::filesystem::path& subpath = "");

	// Returns a read/writable folder that should persist between restarts and ensures subpath exist
	// Note: this function assumes that subpath is only a directory or empty
	extern PXE_NODISCARD std::filesystem::path getPersistentPath(const std::filesystem::path& subpath = "");
}
#endif // !PXENGINE_FILESYSTEM_HELPERS_H_
