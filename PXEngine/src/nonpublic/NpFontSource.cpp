#include "NpFontSource.h"

#include <fstream>

#include "NpEngine.h"
#include "NpFontManager.h"
#include "imgui.h"
#include "NpLogger.h"

namespace pxengine {
	namespace nonpublic {
		NpFontSource::NpFontSource(const std::filesystem::path& path) : _path(path)
		{
			_font = nullptr;
			_fontFileData = nullptr;
			_status = PxeFontStatus::PENDING;
			_size = 13; // Size of default gui font
			_requiredSize = _size;

			std::ifstream file(path, std::ios::binary | std::ios::ate);
			_fontFileSize = file.tellg();
			if (!_fontFileSize) {
				_status = PxeFontStatus::ERROR;
				PXE_WARN("Failed to get font file size");
				return;
			}

			_fontFileData = malloc(_fontFileSize);
			if (!_fontFileData) {
				_status = PxeFontStatus::ERROR;
				PXE_ERROR("Failed to allocate font file buffer");
				return;
			}

			file.seekg(0, std::ios::beg);
			file.read((char*)_fontFileData, _fontFileSize);
			_status = PxeFontStatus::LOADED;
		}

		NpFontSource::~NpFontSource() { }

		void NpFontSource::guiPushFont()
		{
			ImGui::PushFont(_font);
		}

		void NpFontSource::guiPopFont()
		{
			ImGui::PopFont();
		}

		PXE_NODISCARD const std::filesystem::path& NpFontSource::getSourcePath() const
		{
			return _path;
		}

		PXE_NODISCARD PxeFontStatus NpFontSource::getStatus() const
		{
			return _status;
		}

		uint16_t NpFontSource::getFontSize() const
		{
			return _size;
		}

		uint16_t NpFontSource::getRequiredSize() const
		{
			return _requiredSize;
		}

		void NpFontSource::setRequiredSize(float size)
		{
			_requiredSize = size;
		}

		void NpFontSource::onDelete()
		{
			NpEngine::getInstance().getNpFontManager().removeSource(*this);
		}

		void NpFontSource::clearGuiFont()
		{
			_font = nullptr;
		}

		void NpFontSource::updateFontAtlas()
		{
			if (!updateRequired()) return;
			ImFontConfig fontConfig;
			fontConfig.FontDataOwnedByAtlas = false;
			fontConfig.FontData = _fontFileData;
			fontConfig.FontDataSize = _fontFileSize;
			fontConfig.SizePixels = _requiredSize;
			_size = _requiredSize;
			_font = ImGui::GetIO().Fonts->AddFont(&fontConfig);
		}

		bool NpFontSource::updateRequired()
		{
			return _status == PxeFontStatus::LOADED && (!_font || _size != _requiredSize);
		}
	}
}