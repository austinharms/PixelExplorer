#ifndef PXENGINE_FONT_FILE_H_
#define PXENGINE_FONT_FILE_H_
#include <filesystem>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "imgui.h"

namespace pxengine {
	class PxeFontFile : public PxeRefCount
	{
	public:
		void pushFont();
		void popFont();
		PXE_NODISCARD const std::filesystem::path& getPath() const;
		PXE_NODISCARD PxeFontStatus getStatus() const;
		PXE_NODISCARD uint16_t getLoadedSize() const;
		PXE_NODISCARD uint16_t getRequestedSize() const;
		PXE_NODISCARD bool getAtlasUpdateRequested() const;
		void setRequestedSize(uint16_t size);
		void addFontToAtlas(ImFontAtlas& atlas);
		void onDelete() override;
		PxeFontFile(const std::filesystem::path& path);
		virtual ~PxeFontFile();
		PXE_NOCOPY(PxeFontFile);

	private:
		const std::filesystem::path _path;
		void* _fontFileData;
		PxeSize _fontFileSize;
		ImFont* _font;
		uint16_t _imFontSize;
		uint16_t _requestedSize;
		PxeFontStatus _status;
	};
}
#endif // !PXENGINE_FONT_FILE_H_
