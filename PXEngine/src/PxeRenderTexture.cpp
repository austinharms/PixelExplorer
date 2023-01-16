#include "PxeRenderTexture.h"

#include "NpLogger.h"
#include "GL/glew.h"
#include "stb_image.h"

namespace pxengine {
	PxeRenderTexture::PxeRenderTexture() {
		_imageData = nullptr;
		_glTextureId = 0;
		_width = 0;
		_height = 0;
		_textureSlot = 0;
		_stbImage = false;
		_textureLoaded = false;
	}

	void PxeRenderTexture::initializeGl()
	{
		glGenTextures(1, &_glTextureId);
		if (!getValid()) {
			PXE_ERROR("Failed to create GLTexture");
			return;
		}

		uint32_t previousTexture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, (int32_t*)(&previousTexture));
		glBindTexture(GL_TEXTURE_2D, _glTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		updateTexture();
		glBindTexture(GL_TEXTURE_2D, previousTexture);
	}

	void PxeRenderTexture::uninitializeGl()
	{
		_textureLoaded = false;
		glDeleteTextures(1, &_glTextureId);
		_glTextureId = 0;
	}

	void PxeRenderTexture::bind()
	{
		if (!getValid()) {
			PXE_WARN("Attempted to bind invalid PxeRenderTexture");
			return;
		}

		glActiveTexture(GL_TEXTURE0 + _textureSlot);
		glBindTexture(GL_TEXTURE_2D, _glTextureId);
		if (getTexturePending())
			updateTexture();
	}

	void PxeRenderTexture::unbind()
	{
		glActiveTexture(GL_TEXTURE0 + _textureSlot);
#ifdef PXE_DEBUG
		uint32_t previousTexture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, (int32_t*)(&previousTexture));
		if (previousTexture != _glTextureId) {
			PXE_WARN("unbind called on unbound PxeRenderTexture");
		}
#endif // PXE_DEBUG
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void PxeRenderTexture::updateTexture()
	{
		if (!getTexturePending()) return;
		void* pixels = _imageData;
		if (!_stbImage) {
			pixels = static_cast<PxeBuffer*>(_imageData)->getBuffer();
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		freePendingTexture();
	}

	void PxeRenderTexture::freePendingTexture()
	{
		if (!getTexturePending()) return;
		if (_stbImage) {
			stbi_image_free(_imageData);
		}
		else {
			static_cast<PxeBuffer*>(_imageData)->drop();
		}

		_imageData = nullptr;
	}

	PxeRenderTexture::~PxeRenderTexture()
	{
		freePendingTexture();
	}

	void PxeRenderTexture::loadImage(uint16_t width, uint16_t height, PxeBuffer& pixelData)
	{
		pixelData.grab();
		freePendingTexture();
		_imageData = &pixelData;
		_width = width;
		_height = height;
		_stbImage = false;
	}

	void PxeRenderTexture::loadImage(const std::filesystem::path& path)
	{
		int width = 0;
		int height = 0;
		int channels = 0;
		void* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 4);
		if (!pixels) {
			PXE_ERROR("Failed to load texture from " + path.string() + " " + stbi_failure_reason());
			return;
		}

		freePendingTexture();
		_stbImage = true;
		_imageData = pixels;
		_width = static_cast<uint16_t>(width);
		_height = static_cast<uint16_t>(height);
	}

	PXE_NODISCARD bool PxeRenderTexture::getValid() const
	{
		return _glTextureId;
	}

	void PxeRenderTexture::setTextureSlot(uint8_t slot)
	{
		_textureSlot = slot;
	}

	PXE_NODISCARD uint8_t PxeRenderTexture::getTextureSlot() const
	{
		return _textureSlot;
	}

	PXE_NODISCARD bool PxeRenderTexture::getTexturePending() const
	{
		return _imageData;
	}

	PXE_NODISCARD bool PxeRenderTexture::getTextureLoaded() const
	{
		return _textureLoaded;
	}

	PXE_NODISCARD uint32_t PxeRenderTexture::getGlTextureId() const
	{
		return _glTextureId;
	}

	PXE_NODISCARD uint16_t PxeRenderTexture::getTextureWidth() const
	{
		return _width;
	}

	PXE_NODISCARD uint16_t PxeRenderTexture::getTextureHeight() const
	{
		return _height;
	}
}