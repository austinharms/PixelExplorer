#include "PE_asset_manager.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_asset_manager_internal.h"
#include "PE_memory.h"
#include <fstream>

namespace pecore {
	int PE_GetFileBytes(const char* filepath, void*& file_data, size_t& file_size)
	{
		std::ifstream stream;
		size_t size;
		int err = PE_GetFileHandle(filepath, stream, size);
		if (err != PE_ERROR_NONE) {
			return err;
		}

		if (size == 0) {
			return PE_ERROR_EMPTY;
		}

		void* buffer = PE_malloc(size);
		if (buffer == nullptr) {
			return PE_ERROR_OUT_OF_MEMORY;
		}

		stream.read(static_cast<char*>(buffer), size);
		file_data = buffer;
		file_size = size;
		return PE_ERROR_NONE;
	}

	void PE_FreeFileBytes(void* file_data)
	{
		PE_free(file_data);
	}
}
