#include "PxeFont.h"

#include "PxeFontFile.h"

namespace pxengine {
	struct PxeFont::Impl {
		Impl(PxeFontFile& fontFile, uint16_t size, PxeFontScalingMode mode, const glm::i32vec2& windowRef) :
			_size(size), _scalingMode(mode), _windowSize(windowRef), _fontFile(fontFile) {}
		PxeFontFile& _fontFile;
		const uint16_t _size;
		const glm::i32vec2 _windowSize;
		const PxeFontScalingMode _scalingMode;
		float _globalFontScale;
	};

	void PxeFont::pushFont()
	{
		Impl& imp = impl();
		ImGuiIO& io = ImGui::GetIO();
		imp._globalFontScale = io.FontGlobalScale;
		uint16_t size = getScaledPixelSize();
		if (size > imp._fontFile.getRequestedSize()) imp._fontFile.setRequestedSize(size);
		float fontScale = (float)size / (float)imp._fontFile.getLoadedSize();
		io.FontGlobalScale = fontScale;
		imp._fontFile.pushFont();
	}

	void PxeFont::popFont()
	{
		Impl& imp = impl();
		ImGuiIO& io = ImGui::GetIO();
		io.FontGlobalScale = imp._globalFontScale;
		imp._fontFile.popFont();
	}

	PXE_NODISCARD uint16_t PxeFont::getPixelSize() const
	{
		return impl()._size;
	}

	PXE_NODISCARD uint16_t PxeFont::getScaledPixelSize() const
	{
		const Impl& imp = impl();
		switch (imp._scalingMode)
		{
		case PxeFontScalingMode::HEIGHT:
			return static_cast<uint16_t>(floorf(imp._size * (ImGui::GetIO().DisplaySize.y / (float)imp._windowSize.y)));

		case PxeFontScalingMode::WIDTH:
			return static_cast<uint16_t>(floorf(imp._size * (ImGui::GetIO().DisplaySize.x / (float)imp._windowSize.x)));

		case PxeFontScalingMode::SMALLEST:
			return static_cast<uint16_t>(fminf(
				floorf(imp._size * (ImGui::GetIO().DisplaySize.x / (float)imp._windowSize.x)),
				floorf(imp._size * (ImGui::GetIO().DisplaySize.y / (float)imp._windowSize.y))
			));

		case PxeFontScalingMode::LARGEST:
			return static_cast<uint16_t>(fmaxf(
				floorf(imp._size * (ImGui::GetIO().DisplaySize.x / (float)imp._windowSize.x)),
				floorf(imp._size * (ImGui::GetIO().DisplaySize.y / (float)imp._windowSize.y))
			));

		case PxeFontScalingMode::UNSCALED:
		default:
			return imp._size;
		}
	}

	PXE_NODISCARD const std::filesystem::path& PxeFont::getPath() const
	{
		return impl()._fontFile.getPath();
	}

	PXE_NODISCARD PxeFontScalingMode PxeFont::getScalingMode() const
	{
		return impl()._scalingMode;
	}

	PXE_NODISCARD const glm::i32vec2& PxeFont::getReferenceWindowSize() const
	{
		return impl()._windowSize;
	}

	PXE_NODISCARD PxeFontStatus PxeFont::getStatus() const
	{
		return impl()._fontFile.getStatus();
	}

	PxeFont::PxeFont(PxeFontFile& fontFile, uint16_t size, PxeFontScalingMode mode, const glm::i32vec2& windowRef) {
		static_assert(sizeof(Impl) <= STORAGE_SIZE, "PxeFont::STORAGE_SIZE need be changed");
		static_assert(STORAGE_ALIGN == alignof(Impl), "PxeFont::STORAGE_ALIGN need be changed");

		new(&_storage) Impl(fontFile, size, mode, windowRef);
		fontFile.grab();
	}

	PxeFont::~PxeFont()
	{
		impl()._fontFile.drop();
		impl().~Impl();
	}
}