#ifndef PXENGINE_NONPUBLIC_FONT_MANAGER_H_
#define PXENGINE_NONPUBLIC_FONT_MANAGER_H_
#include <filesystem>
#include <unordered_map>

#include "PxeTypes.h"
#include "PxeFontManager.h"
#include "PxeFont.h"
#include "NpFont.h"
#include "NpFontSource.h"

namespace pxengine {
	namespace nonpublic {
		class NpFontManager : public PxeFontManager
		{
		public:
			NpFontManager();
			virtual ~NpFontManager();
			PXE_NODISCARD PxeFont* loadFont(const std::filesystem::path& path, uint16_t size, PxeFontScalingMode scaleMode, const glm::i32vec2& referenceWindowSize) override;
			void removeSource(NpFontSource& source);
			ImFontAtlas* getFontAtlas();
			void updateFontAtlas();

		private:
			ImFontAtlas _fontAtlas;
			std::unordered_map<std::filesystem::path, NpFontSource*> _fontSources;
		};
	}
}

#endif // !PXENGINE_NONPUBLIC_FONT_MANAGER_H_
