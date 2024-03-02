#ifndef PE_ASSET_MANAGER_H_
#define PE_ASSET_MANAGER_H_
#include <filesystem>

namespace pecore {
	// Sets out_stream to the file handle and sets out_size to the size of the file in bytes, returns a PE_ERROR_* value,
	// out_stream and out_size will be unchanged if this fails
	// Note: the file handle may be in the middle of an asset pack and therefore should not be set to the start, end or change the file in any way
	int PE_GetFileHandle(const std::filesystem::path& filepath, std::ifstream& out_stream, size_t& out_size);
}
#endif // !PE_ASSET_MANAGER_H_
