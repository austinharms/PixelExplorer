#include "GLTexture.h"

#include <malloc.h>

#include "GL/glew.h"
#include "Logger.h"

namespace pixelexplorer::rendering {
	GLTexture::GLTexture(uint32_t width, uint32_t height)
	{
		_pixelBuffer = (uint8_t*)malloc(width * height * 4);
		_width = width;
		_height = height;
		_stbImage = false;
		_glTextureId = 0;
	}

	GLTexture::GLTexture(const std::string& path)
	{
		int width = 0;
		int height = 0;
		int channels = 0;
		_pixelBuffer = stbi_load(path.c_str(), &width, &height, &channels, 4);
		if (_pixelBuffer == nullptr)
			Logger::warn("Failed to load image from " + path);

		_width = width;
		_height = height;
		_stbImage = true;
		_glTextureId = 0;
	}

	GLTexture::~GLTexture()
	{
		if (_pixelBuffer != nullptr) {
			if (_stbImage) {
				stbi_image_free(_pixelBuffer);
			}
			else {
				free(_pixelBuffer);
			}

			_pixelBuffer = nullptr;
		}
	}

	void GLTexture::bind(uint8_t slot)
	{
		if (getInitialized()) {
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, _glTextureId);
		}
	}

	void GLTexture::onInitialize()
	{
		glGenTextures(1, &_glTextureId);
		glBindTexture(GL_TEXTURE_2D, _glTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		onUpdate();
	}

	void GLTexture::onTerminate()
	{
		glDeleteTextures(1, &_glTextureId);
		_glTextureId = 0;
	}

	void GLTexture::onUpdate()
	{
		if (_pixelBuffer == nullptr) return;
		glBindTexture(GL_TEXTURE_2D, _glTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixelBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}
