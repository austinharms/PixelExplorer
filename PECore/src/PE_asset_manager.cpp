#include "PE_asset_manager.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_memory.h"
#include <fstream>

namespace pe {
	class TextAssetImp : public TextAsset {
	public:
		TextAssetImp(AssetSize size, char* data) :
			file_size_(size),
			file_data_(data) {
			// Allow for the size required to appending a null value at the end
			PE_STATIC_ASSERT((sizeof(*this) + 1) <= ASSET_MAX_PADDING, PE_TEXT("TextAssetImp size too big"));
		}

		PE_NODISCARD AssetSize GetSize() PE_OVERRIDE {
			return file_size_;
		}

		PE_NODISCARD const char* GetText() PE_OVERRIDE {
			return file_data_;
		}

		void OnDrop() PE_OVERRIDE {
			PE_free(this);
		}

	private:
		AssetSize file_size_;
		char* file_data_;
	};

	PE_NODISCARD TextAsset* TextAsset::LoadFile(const char* filepath, ErrorCode* error_out) {
		TextAssetImp* asset = nullptr;
		try {
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
			if (stream_size > ASSET_MAX_SIZE) {
				if (error_out) *error_out = PE_ERROR_TOO_BIG;
				return nullptr;
			}

			AssetSize file_size = stream_size;
			if (file_size == 0 || stream.fail()) {
				if (error_out) *error_out = PE_ERROR_EMPTY;
				return nullptr;
			}

			stream.clear();
			asset = static_cast<TextAssetImp*>(PE_malloc(sizeof(TextAssetImp) + file_size + 1));
			if (!asset) {
				if (error_out) *error_out = PE_ERROR_OUT_OF_MEMORY;
				return nullptr;
			}

			stream.read(reinterpret_cast<char*>(asset + 1), stream_size);
			// null terminate the buffer
			PE_STATIC_ASSERT(sizeof(char) == 1, PE_TEXT("Expected char to be 1 byte"));
			reinterpret_cast<char*>(asset + 1)[asset->GetSize()] = 0;
			new(asset) TextAssetImp(file_size, asset + 1);
			if (error_out) *error_out = PE_ERROR_NONE;
			return static_cast<TextAsset*>(asset);
		}
		catch (...) {
			if (asset) PE_free(asset);
			if (error_out) *error_out = PE_ERROR_GENERAL;
			return nullptr;
		}
	}
}
