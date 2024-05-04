#include "PE_asset_manager.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_memory.h"
#include <fstream>

namespace pe {
	class FileAssetImp : public FileAsset {
	public:
		FileAssetImp(FileSize size, void* data) :
			file_size_(size),
			file_data_(data) {
		}

		PE_NODISCARD FileSize GetSize() PE_OVERRIDE {
			return file_size_;
		}

		PE_NODISCARD const void* GetBytes() PE_OVERRIDE {
			return file_data_;
		}

		void OnDrop() PE_OVERRIDE {
			PE_free(this);
		}

	private:
		FileSize file_size_;
		void* file_data_;
	};

	PE_NODISCARD FileAsset* FileAsset::LoadFile(const char* filepath, ErrorCode* error_out) {
		if (!filepath) {
			if (error_out) *error_out = PE_ERROR_INVALID_PARAMETERS;
			return nullptr;
		}

		std::ifstream stream(filepath, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
		if (stream.fail()) {
			if (error_out) *error_out = PE_ERROR_NOT_FOUND;
			return nullptr;
		}

		// We should already be at the end but just in case
		stream.seekg(0, std::ios::end);
		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		std::streamsize stream_size = end - stream.tellg();
		if (stream_size > FILE_SIZE_MAX) {
			if (error_out) *error_out = PE_ERROR_TOO_BIG;
			return nullptr;
		}

		FileSize file_size = stream_size;
		if (file_size == 0 || stream.fail()) {
			if (error_out) *error_out = PE_ERROR_EMPTY;
			return nullptr;
		}

		stream.clear();
		FileAsset* asset = static_cast<FileAsset*>(PE_malloc(sizeof(FileAsset) + file_size));
		if (!asset) {
			if (error_out) *error_out = PE_ERROR_OUT_OF_MEMORY;
			return nullptr;
		}

		stream.read(reinterpret_cast<char*>(asset + 1), stream_size);
		new(asset) FileAssetImp(file_size, asset + 1);
		if (error_out) *error_out = PE_ERROR_NONE;
		return static_cast<FileAsset*>(asset);
	}
}
