#ifndef RENDERING_TEXTURE_H
#define RENDERING_TEXTURE_H

#include "RefCounted.h"
#include "stdint.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_FAILURE_USERMSG
#define STBI_MAX_DIMENSIONS 0xffff
#include "stb_image/stb_image.h"

namespace px::rendering {
	class Texture : public RefCounted
	{
	public:
		const uint8_t* Pixels;
		const uint16_t Width;
		const uint16_t Height;

		Texture* loadTexture(const char* path);
		~Texture();
	private:
		Texture(uint16_t width, uint16_t height, uint8_t* data);
	};
}
#endif // !TEXTURE_H
