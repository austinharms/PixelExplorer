#ifndef PXENGINE_RENDERTEXTURE_H_
#define PXENGINE_RENDERTEXTURE_H_
#include <filesystem>

#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "PxeBuffer.h"

namespace pxengine {
	// Wrapper class for a GL_TEXTURE_2D
	class PxeRenderTexture : public PxeGLAsset
	{
	public:
		PxeRenderTexture();
		virtual ~PxeRenderTexture();

		void bind() override;

		void unbind() override;

		// TODO Add support for more texture formats
		// Loads pixelData into the PxeRenderTexture
		// Note: pixelData is expected to be in the format RGBA with each component 8 bits
		// Note: the GL_TEXTURE_2D will only be updated on the next call to bind()
		void loadImage(uint16_t width, uint16_t height, PxeBuffer& pixelData);

		// Loads an image from file system into the PxeRenderTexture
		// Note: currently only configured for PNG images 
		// Note: the GL_TEXTURE_2D will only be updated on the next call to bind()
		void loadImage(const std::filesystem::path& path);

		// Returns if the GL_TEXTURE_2D is created/valid
		PXE_NODISCARD bool getValid() const;

		// Sets the glActiveTexture when bind is called
		// Note: only 0-15 (inclusive) are guaranteed to work
		void setTextureSlot(uint8_t slot);

		// Returns the glActiveTexture set when bind is called
		PXE_NODISCARD uint8_t getTextureSlot() const;

		// Returns true if there is texture waiting to be loaded
		PXE_NODISCARD bool getTexturePending() const;

		// Returns if there is currently a texture loaded
		PXE_NODISCARD bool getTextureLoaded() const;

		// Returns the id of the GL_TEXTURE_2D
		PXE_NODISCARD uint32_t getGlTextureId() const;

		// Returns the width of the last texture
		// Note: if there is a texture pending this will return the width of the pending texture
		PXE_NODISCARD uint16_t getTextureWidth() const;

		// Returns the height of the last texture
		// Note: if there is a texture pending this will return the height of the pending texture
		PXE_NODISCARD uint16_t getTextureHeight() const;

		PxeRenderTexture(const PxeRenderTexture& other) = delete;
		PxeRenderTexture operator=(const PxeRenderTexture& other) = delete;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		// Load the pending texture into the GL_TEXTURE_2D
		// Note: assumes the texture is bound and a valid OpenGl context
		void updateTexture();

		// Free the memory used by the pending texture and clear it
		void freePendingTexture();

		void* _imageData;
		uint32_t _glTextureId;
		uint16_t _width;
		uint16_t _height;
		uint8_t _textureSlot;
		bool _stbImage;
		bool _textureLoaded;
	};
}
#endif // !PXENGINE_RENDERTEXTURE_H_
