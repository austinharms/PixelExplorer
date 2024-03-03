#ifndef PE_ASSET_MANAGER_H_
#define PE_ASSET_MANAGER_H_
#include <filesystem>

namespace pecore {
	// Sets file_data to a buffer containing the file contents and sets out_size to the size of the file in bytes, returns a PE_ERROR_* value,
	// Note: you must be free file_data using PE_FreeFileBytes, file_data and out_size will be unchanged if this fails
	int PE_GetFileBytes(const char* filepath, void*& file_data, size_t& file_size);
	void PE_FreeFileBytes(void* file_data);
}
#endif // !PE_ASSET_MANAGER_H_
