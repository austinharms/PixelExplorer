#include "NpFont.h"

#include <math.h>

#include "imgui.h"

namespace pxengine {
	namespace nonpublic {
		NpFont::NpFont(NpFontSource& source, uint16_t size, PxeFontScalingMode mode, const glm::i32vec2& windowRef) : _size(size), _scaleMode(mode), _windowRefSize(windowRef) {
			source.grab();
			_source = &source;
			_oldFontScale = 0;
		}

		NpFont::~NpFont() {
			_source->drop();
		}

		void NpFont::guiPushFont()
		{
			ImGuiIO& io = ImGui::GetIO();
			_oldFontScale = io.FontGlobalScale;
			uint16_t size = getScaledPixelSize();
			if (size > _source->getRequiredSize())
				_source->setRequiredSize(size);
			float fontScale = (float)size / (float)_source->getFontSize();
			io.FontGlobalScale = fontScale;
			_source->guiPushFont();
		}

		void NpFont::guiPopFont()
		{
			ImGuiIO& io = ImGui::GetIO();
			io.FontGlobalScale = _oldFontScale;
			_source->guiPopFont();
		}

		uint16_t NpFont::getPixelSize() const
		{
			return _size;
		}

		uint16_t NpFont::getScaledPixelSize() const
		{
			switch (_scaleMode)
			{
			case PxeFontScalingMode::HEIGHT:
				return static_cast<uint16_t>(floorf(_size * (ImGui::GetIO().DisplaySize.y / (float)_windowRefSize.y)));

			case PxeFontScalingMode::WIDTH:
				return static_cast<uint16_t>(floorf(_size * (ImGui::GetIO().DisplaySize.x / (float)_windowRefSize.x)));

			case PxeFontScalingMode::SMALLEST:
				return static_cast<uint16_t>(fminf(
						floorf(_size * (ImGui::GetIO().DisplaySize.x / (float)_windowRefSize.x)),
						floorf(_size * (ImGui::GetIO().DisplaySize.y / (float)_windowRefSize.y))
					));

			case PxeFontScalingMode::LARGEST:
				return static_cast<uint16_t>(fmaxf(
					floorf(_size * (ImGui::GetIO().DisplaySize.x / (float)_windowRefSize.x)),
					floorf(_size * (ImGui::GetIO().DisplaySize.y / (float)_windowRefSize.y))
				));

			case PxeFontScalingMode::UNSCALED:
			default:
				return _size;
			}
		}

		const std::filesystem::path& NpFont::getFontSource() const
		{
			return _source->getSourcePath();
		}

		PxeFontScalingMode NpFont::getScalingMode() const
		{
			return _scaleMode;
		}

		const glm::i32vec2& NpFont::getReferenceWindowSize() const
		{
			return _windowRefSize;
		}

		PxeFontStatus NpFont::getStatus() const
		{
			return _source->getStatus();
		}
	}
}