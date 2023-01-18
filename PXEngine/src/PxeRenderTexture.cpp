#include "PxeRenderTexture.h"

#include "GL/glew.h"
#include "NpLogger.h"

namespace pxengine {
	PxeRenderTexture::PxeRenderTexture(uint32_t width, uint32_t height) {
		_glFramebufferId = 0;
		_glRenderTextureId = 0;
		_glDepthBuffer = 0;
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, _glDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
	}

	uint32_t PxeRenderTexture::getGlTextureId() const
	{
		return _glRenderTextureId;
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

		uint32_t previousBuffer;
		glGetIntegerv(GL_RENDERBUFFER_BINDING, (int32_t*)(&previousBuffer));
		glGenRenderbuffers(1, &_glDepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, _glDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
		glBindRenderbuffer(GL_RENDERBUFFER, previousBuffer);

		glGenFramebuffers(1, &_glFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, _glFramebufferId);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _glDepthBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _glRenderTextureId, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			setErrorStatus();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PxeRenderTexture::uninitializeGl()
	{
		glDeleteFramebuffers(1, &_glFramebufferId);
		_glFramebufferId = 0;
		glDeleteTextures(1, &_glRenderTextureId);
		_glRenderTextureId = 0;
		glDeleteRenderbuffers(1, &_glDepthBuffer);
		_glDepthBuffer = 0;
	}

	void PxeRenderTexture::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _glFramebufferId);
	}

	void PxeRenderTexture::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}