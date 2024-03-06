#ifndef PE_ASSET_MANAGER_INTERNAL_H_
#define PE_ASSET_MANAGER_INTERNAL_H_
#include <filesystem>

namespace pecore {
	// This must be internal due to ABI compatibility and memory allocation across dll boundaries

	// Sets out_stream to the file handle and sets end_pos to the end position of the file, returns a PE_ERROR_* value,
	// out_stream and end_pos will be unchanged if this fails
	// Note: the file handle may be in the middle of an asset pack and therefore should not be set to the start, end or change the file in any way
	int PE_GetFileHandle(const std::filesystem::path& filepath, std::ifstream& out_stream, std::streampos& end_pos);
}
#endif // !PE_ASSET_MANAGER_INTERNAL_H_
