#include "NpFontManager.h"

#include <new>

#include "NpLogger.h"
#include "NpEngine.h"
#include "NpGuiGlBackend.h"

namespace pxengine {
	namespace nonpublic {
		NpFontManager::NpFontManager()
		{
		}

		NpFontManager::~NpFontManager()
		{
		}

		PxeFont* NpFontManager::loadFont(const std::filesystem::path& path, uint16_t size, PxeFontScalingMode scaleMode, const glm::i32vec2& referenceWindowSize)
		{
			auto sourceItr = _fontSources.find(path);
			if (sourceItr != _fontSources.end()) {
				NpFont* font = new(std::nothrow) NpFont(*(sourceItr->second), size, scaleMode, referenceWindowSize);
				if (!font) {
					PXE_ERROR("Failed to create PxeFont");
					return nullptr;
				}

				if (sourceItr->second->getRequiredSize() < size)
					sourceItr->second->setRequiredSize(size);
				return font;
			}
			else {
				NpFontSource* src = new(std::nothrow) NpFontSource(path);
				if (!src) {
					PXE_ERROR("Failed to create font source");
					return nullptr;
				}

				_fontSources.emplace(path, src);
				NpFont* font = new(std::nothrow) NpFont(*src, size, scaleMode, referenceWindowSize);
				if (!font) {
					PXE_ERROR("Failed to create PxeFont");
					return nullptr;
				}

				src->setRequiredSize(size);
				return font;
			}
		}

		void NpFontManager::removeSource(NpFontSource& source)
		{
			_fontSources.erase(source.getSourcePath());
		}

		ImFontAtlas* NpFontManager::getFontAtlas()
		{
			return &_fontAtlas;
		}

		void NpFontManager::updateFontAtlas()
		{
			bool updateRequired = false;
			for (auto pair : _fontSources) {
				if (pair.second->updateRequired()) {
					updateRequired = true;
					break;
				}
			}

			if (updateRequired) {
				_fontAtlas.Clear();
				_fontAtlas.AddFontDefault();
				for (auto pair : _fontSources) {
					pair.second->clearGuiFont();
					pair.second->updateFontAtlas();
				}

				NpEngine::getInstance().getNpGuiBackend().rebuildFontTexture();
				PXE_INFO("Font atlas rebuilt");
			}
		}
	}
}