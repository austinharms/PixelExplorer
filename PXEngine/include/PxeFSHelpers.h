#include <filesystem>

#ifndef PXENGINE_FILESYSTEM_HELPERS_H_
#define PXENGINE_FILESYSTEM_HELPERS_H_
namespace pxengine {
	// returns the path of the executable, including the executable filename
	extern _NODISCARD std::filesystem::path getExecutablePath();

	// returns the path of the executable, excluding the filename
	extern _NODISCARD std::filesystem::path getExecutableDirectory();

	// returns the path to the application asset folder
	// note this folder may not be writable, if it is writable this also ensures subpath exist
	// this function also assumes that subpath is only a directory or empty
	extern _NODISCARD std::filesystem::path getAssetPath(const std::filesystem::path& subpath = "");

	// returns a read/writable folder that should persist between restarts also ensures subpath exist
	// note this function assumes that subpath is only a directory or empty
	extern _NODISCARD std::filesystem::path getPersistentPath(const std::filesystem::path& subpath = "");
}
#endif // !PXENGINE_FILESYSTEM_HELPERS_H_
