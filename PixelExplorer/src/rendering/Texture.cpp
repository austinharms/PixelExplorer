#include "Texture.h"

#include <malloc.h>

#include "GL/glew.h"
#include "Logger.h"

namespace pixelexplorer::rendering {
	Texture::Texture(uint32_t width, uint32_t height)
	{
		_pixelBuffer = (uint8_t*)malloc(width * height * 4);
		_width = width;
		_height = height;
		_stbImage = false;
		_glTextureId = 0;
		_hasGlObject = false;
	}

	Texture::Texture(const std::string& path)
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
		_hasGlObject = false;
	}

	Texture::~Texture()
	{
		if (_pixelBuffer != nullptr) {
			if (_stbImage) {
				stbi_image_free(_pixelBuffer);
				_pixelBuffer = nullptr;
			}
			else {
				free(_pixelBuffer);
				_pixelBuffer = nullptr;
			}
		}

		if (_hasGlObject) {
			glDeleteTextures(1, &_glTextureId);
			_glTextureId = 0;
		}
	}

	void Texture::createGlTexture()
	{
		if (_pixelBuffer == nullptr) {
			Logger::warn("Attempted to create texture with NULL pixel buffer");
			return;
		}

		if (!_hasGlObject) {
			_hasGlObject = true;
			glGenTextures(1, &_glTextureId);
			glBindTexture(GL_TEXTURE_2D, _glTextureId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, _glTextureId);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixelBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture::deleteGlTexture()
	{
		if (_hasGlObject) {
			glDeleteTextures(1, &_glTextureId);
			_glTextureId = 0;
			_hasGlObject = false;
		}
	}

	void Texture::bind(uint8_t slot)
	{
		if (_hasGlObject) {
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, _glTextureId);
		}
	}
}
