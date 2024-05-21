#ifndef PE_ASSET_MANAGER_H_
#define PE_ASSET_MANAGER_H_
#include "PE_defines.h"
#include "PE_referenced.h"
#include "PE_errors.h"
#include <filesystem>

namespace pe {
	class Asset : public Referenced {
	public:
		typedef uint64_t AssetSize;
		// Allow for up to 64 bytes of internal data and or padding
		static constexpr AssetSize ASSET_MAX_PADDING = 64;
		static constexpr AssetSize ASSET_MAX_SIZE = 0xFFFFFFFFFFFFFFFF - ASSET_MAX_PADDING;
		virtual ~Asset() = default;
	};

	// Referenced class for loading and storing binary data
	class BinaryAsset : public Asset {
	public:
		virtual ~BinaryAsset() = default;
		PE_NOCOPY(BinaryAsset);

		// Loads the file from filepath and returns a new BinaryAsset object or, nullptr on error
		// Sets error_out to an ErrorCode on error or PE_ERROR_NONE on success
		PE_FORCEINLINE PE_NODISCARD static BinaryAsset* LoadFile(const std::filesystem::path& filepath, ErrorCode* error_out) {
			try {
				const std::string path_string(std::move(filepath.generic_u8string()));
				return LoadFile(path_string.c_str(), error_out);
			}
			catch(...) {
				return nullptr;
			}
		}

		// Loads the file from filepath and returns a new BinaryAsset object or, nullptr on error
		// Sets error_out to an ErrorCode on error or PE_ERROR_NONE on success
		PE_NODISCARD static BinaryAsset* LoadFile(const char* filepath, ErrorCode* error_out);
		// Returns the size of the buffer in bytes
		virtual PE_NODISCARD AssetSize GetSize() = 0;
		// Returns a pointer to the file data
		virtual PE_NODISCARD const void* GetBytes() = 0;

	protected:
		BinaryAsset() = default;
	};

	// Referenced class for loading and storing text file data
	class TextAsset : public Asset {
	public:
		virtual ~TextAsset() = default;
		PE_NOCOPY(TextAsset);

		// Loads the file from filepath and returns a new TextAsset object or, nullptr on error
		// Sets error_out to an ErrorCode on error or PE_ERROR_NONE on success
		PE_FORCEINLINE PE_NODISCARD static TextAsset* LoadFile(const std::filesystem::path& filepath, ErrorCode* error_out) {
			try {
				const std::string path_string(std::move(filepath.generic_u8string()));
				return LoadFile(path_string.c_str(), error_out);
			}
			catch (...) {
				return nullptr;
			}
		}

		// Loads the file from filepath and returns a new TextAsset object or, nullptr on error
		// Sets error_out to an ErrorCode on error or PE_ERROR_NONE on success
		PE_NODISCARD static TextAsset* LoadFile(const char* filepath, ErrorCode* error_out);
		// Returns the size of the buffer not including the null terminator in bytes
		virtual PE_NODISCARD AssetSize GetSize() = 0;
		// Returns a pointer to the file data
		// Note: a 0 value char aka null is appended to the end of the buffer to allow use with C style string functions and it is assumed the file contains UTF-8 encoded text
		virtual PE_NODISCARD const char* GetText() = 0;
		// TODO Comment this function
		virtual ErrorCode GetLine(char* buffer, uint64_t buffer_size, char delimiter = '\n') = 0;

	protected:
		TextAsset() = default;
	};
}
#endif // !PE_ASSET_MANAGER_H_
