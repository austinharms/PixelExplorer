#include "PxeRenderTexture.h"

#include "GL/glew.h"
#include "PxeLogger.h"

namespace pxengine {
	PxeRenderTexture::PxeRenderTexture(uint32_t width, uint32_t height, bool delayAssetInitialization) : PxeGLAsset(delayAssetInitialization) {
		_glRenderTextureId = 0;
		_width = width;
		_height = height;
	}

	PxeRenderTexture::~PxeRenderTexture() { }

	void PxeRenderTexture::resize(uint32_t width, uint32_t height)
	{
		if (width == _width && height == _height) return;
		if (getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
			PXE_ERROR("Attempted to resize PxeRenderTexture thats status was not INITIALIZED");
			return;
		}

		_width = width;
		_height = height;
		glBindTexture(GL_TEXTURE_2D, _glRenderTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}

	uint32_t PxeRenderTexture::getGlTextureId() const
	{
		return _glRenderTextureId;
	}

	uint32_t PxeRenderTexture::getWidth() const
	{
		return _width;
	}

	uint32_t PxeRenderTexture::getHeight() const
	{
		return _height;
	}

	void PxeRenderTexture::initializeGl()
	{
		uint32_t previousTexture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, (int32_t*)(&previousTexture));
		glGenTextures(1, &_glRenderTextureId);
		glBindTexture(GL_TEXTURE_2D, _glRenderTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, previousTexture);
	}

	void PxeRenderTexture::uninitializeGl()
	{
		glDeleteTextures(1, &_glRenderTextureId);
		_glRenderTextureId = 0;
	}
}