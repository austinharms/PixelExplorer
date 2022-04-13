#include "Texture.h"
#include "Logger.h"
#include "string"

namespace px::rendering {
	Texture* Texture::loadTexture(const char* path)
	{
		int32_t x, y, n;
		void* data = stbi_load(path, &x, &y, &n, 4);
		return new Texture((uint16_t)x, (uint16_t)y, (uint8_t*)data);
	}

	Texture::~Texture()
	{
		stbi_image_free((void*)Pixels);
	}

	Texture::Texture(uint16_t width, uint16_t height, uint8_t* data) : Width(width), Height(height), Pixels(data) {
		if (data == nullptr) {
			Logger::error(std::string("Failed to load texture: ") + stbi_failure_reason());
		}
	}
}