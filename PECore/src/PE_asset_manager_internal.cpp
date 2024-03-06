#include "PE_asset_manager_internal.h"
#include "PE_errors.h"
#include <fstream>

namespace pecore {
	// TODO Look for files in "asset packs"
	int PE_GetFileHandle(const std::filesystem::path& filepath, std::ifstream& out_stream, std::streampos& end_pos)
	{
		std::ifstream stream(filepath, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
		if (stream.fail()) {
			return PE_ERROR_NOT_FOUND;
		}

		// We should already be at the end but just in case
		stream.seekg(0, std::ios::end);
		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		size_t file_size = static_cast<size_t>(end - stream.tellg());
		if (file_size == 0 || stream.fail()) {
			return PE_ERROR_EMPTY;
		}

		stream.clear();
		out_stream = std::move(stream);
		end_pos = end;
		return PE_ERROR_NONE;
	}
}
