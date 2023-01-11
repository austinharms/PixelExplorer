#include <stdint.h>
#include <filesystem>
#include <new>

#include "PxeGLAsset.h"
#include "PxeBuffer.h"

#ifndef PXENGINE_RENDERTEXTURE_H_
#define PXENGINE_RENDERTEXTURE_H_
namespace pxengine {
	class PxeRenderTexture : public PxeGLAsset
	{
	public:
		PxeRenderTexture();
		virtual ~PxeRenderTexture();

		void bind() override;

		void unbind() override;

		// Loads pixelData into the PxeRenderTexture
		// note pixelData is expected to be in the format RGBA with each component 8 bits
		// TODO Add support for more texture formats
		void loadImage(uint16_t width, uint16_t height, PxeBuffer& pixelData);

		// Loads an image from file system into the PxeRenderTexture
		// note Currently only configured for PNG images 
		void loadImage(const std::filesystem::path& path);

		// returns if the internal gl texture is valid
		bool getValid() const;

		// Sets the gl texture slot the PxeRenderTexture is bound to when bind is called
		void setTextureSlot(uint8_t slot);

		// returns the gl texture slot the PxeRenderTexture is bound to when bind is called
		uint8_t getTextureSlot() const;

		// returns true if there is texture waiting to be loaded
		bool getTexturePending() const;

		// returns true if there is currently a texture loaded
		bool getTextureLoaded() const;

		// returns the id of the internal gl texture
		uint32_t getGlTextureId() const;

		// note if there is a texture pending this will return the width of the pending texture
		uint16_t getTextureWidth() const;

		// note if there is a texture pending this will return the height of the pending texture
		uint16_t getTextureHeight() const;

		PxeRenderTexture(const PxeRenderTexture& other) = delete;
		PxeRenderTexture operator=(const PxeRenderTexture& other) = delete;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		void updateTexture();
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
