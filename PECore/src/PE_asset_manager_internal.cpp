#include "PE_asset_manager_internal.h"
#include "PE_errors.h"
#include <fstream>

namespace pecore {
	// TODO Look for files in "asset packs"
	int PE_GetFileHandle(const std::filesystem::path& filepath, std::ifstream& out_stream, size_t& out_size)
	{
		std::ifstream stream(filepath, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
		if (stream.fail()) {
			return PE_ERROR_NOT_FOUND;
		}

		stream.seekg(std::ios::end);
		std::streampos end_pos = stream.tellg();
		stream.seekg(std::ios::beg);
		size_t file_size = static_cast<size_t>(end_pos - stream.tellg());
		if (file_size == 0 || stream.fail()) {
			return PE_ERROR_EMPTY;
		}

		out_size = file_size;
		out_stream = std::move(stream);
		return PE_ERROR_NONE;
	}
}
