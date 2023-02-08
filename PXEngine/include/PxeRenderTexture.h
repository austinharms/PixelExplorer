#ifndef PXENGINE_RENDERTEXTURE_H_
#define PXENGINE_RENDERTEXTURE_H_
#include "PxeTypes.h"
#include "PxeGLAsset.h"

namespace pxengine {
	// TODO Comment file
	class PxeRenderTexture : public PxeGLAsset
	{
	public:
		PxeRenderTexture(uint32_t width, uint32_t height, bool delayAssetInitialization = false);
		virtual ~PxeRenderTexture();

		// Binds the GL_FRAMEBUFFER
		void bind() override;

		// Binds 0 as GL_FRAMEBUFFER
		void unbind() override;

		// Resizes the PxeRenderTexture to {width} by {height} pixels
		// Note: this method assumes the PxeRenderTexture is bound and a valid OpenGl context
		// Note: this overwrites the active glTexture and glRenderbuffer
		void resize(uint32_t width, uint32_t height);

		uint32_t getGlTextureId() const;

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		uint32_t _glFramebufferId;
		uint32_t _glRenderTextureId;
		uint32_t _glDepthBuffer;
		uint32_t _width;
		uint32_t _height;
	};
}
#endif // !PXENGINE_RENDERTEXTURE_H_