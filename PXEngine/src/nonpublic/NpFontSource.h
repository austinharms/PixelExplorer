#ifndef PXENGINE_NONPUBLIC_FONT_SOURCE_H_
#define PXENGINE_NONPUBLIC_FONT_SOURCE_H_
#include <filesystem>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "imgui.h"

namespace pxengine {
	namespace nonpublic {
		class NpFontSource : public PxeRefCount
		{
		public:
			NpFontSource(const std::filesystem::path& path);
			virtual ~NpFontSource();
			void guiPushFont();
			void guiPopFont();
			PXE_NODISCARD const std::filesystem::path& getSourcePath() const;
			PXE_NODISCARD PxeFontStatus getStatus() const;
			PXE_NODISCARD uint16_t getFontSize() const;
			PXE_NODISCARD uint16_t getRequiredSize() const;
			void setRequiredSize(float size);
			void onDelete() override;
			void addFontToAtlas(ImFontAtlas& atlas);
			bool fontAtlasUpdateRequired();

		private:
			const std::filesystem::path _path;
			ImFont* _font;
			void* _fontFileData;
			size_t _fontFileSize;
			PxeFontStatus _status;
			uint16_t _size;
			uint16_t _requiredSize;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_FONT_SOURCE_H_
