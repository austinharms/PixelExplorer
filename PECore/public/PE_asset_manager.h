#ifndef PE_ASSET_MANAGER_H_
#define PE_ASSET_MANAGER_H_
#include "PE_defines.h"
#include "PE_referenced.h"
#include "PE_errors.h"
#include <filesystem>

namespace pe {
	class Asset : public Referenced {
	public:
		virtual ~Asset() = default;
	};

	// Referenced class for loading and storing file data
	class FileAsset : public Asset {
	public:
		typedef uint64_t FileSize;
		static constexpr FileSize FILE_SIZE_MAX = UINT64_MAX;
		virtual ~FileAsset();
		PE_NOCOPY(FileAsset);

		// Loads the file from filepath and returns a new FileAsset object or, nullptr on error
		// Sets error_out to an ErrorCode on error or PE_ERROR_NONE on success
		PE_NODISCARD static FileAsset* LoadFile(const std::filesystem::path& filepath, ErrorCode* error_out) {
			return LoadFile(filepath.c_str(), error_out);
		}

		// Loads the file from filepath and returns a new FileAsset object or, nullptr on error
		PE_NODISCARD static FileAsset* LoadFile(const char* filepath, ErrorCode* error_out);
		virtual PE_NODISCARD FileSize GetSize() = 0;
		virtual PE_NODISCARD const void* GetBytes() = 0;

	protected:
		FileAsset() = default;
	};
}
#endif // !PE_ASSET_MANAGER_H_
