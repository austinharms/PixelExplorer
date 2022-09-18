#include <filesystem>

#ifndef PIXELEXPLORER_OSHELPERS_H_
#define PIXELEXPLORER_OSHELPERS_H_
namespace pixelexplorer::OSHelper {
	// returns the path of the executable, including the filename
	inline _NODISCARD std::filesystem::path getExecutablePath();

	// returns the path of the executable, excluding the filename
	inline _NODISCARD std::filesystem::path getExecutableDirectory();

	// returns the path to the application asset folder
	// note this folder may not be writable, if it is ensures subpath exist (creates directory if not)
	// this function assumes that subpath is only a directory or empty
	inline _NODISCARD std::filesystem::path getAssetPath(const std::filesystem::path& subpath = "");

	// returns a read/writable folder that should persist between restarts
	// also ensures subpath exist (creates directory if not)
	// this function assumes that subpath is only a directory or empty
	inline _NODISCARD std::filesystem::path getPersistentPath(const std::filesystem::path& subpath = "");
}

#include "OSHelpers.inl"
#endif // PIXELEXPLORER_OSHELPERS_H_
