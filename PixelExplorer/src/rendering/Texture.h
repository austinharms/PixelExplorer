#include <stdint.h>
#include <string>

#include "RefCount.h"
#include "stb_image.h"

#ifndef PIXELEXPLORER_RENDERING_TEXTURE_H_
#define PIXELEXPLORER_RENDERING_TEXTURE_H_
namespace pixelexplorer::rendering {
	class Texture : public RefCount
	{
	public:
		Texture(uint32_t width, uint32_t height);
		Texture(const std::string& path);
		virtual ~Texture();
		void createGlTexture();
		void deleteGlTexture();
		void bind(uint8_t slot = 0);
		inline uint32_t getGlId() const { return _glTextureId; }

	private:
		uint8_t* _pixelBuffer;
		uint32_t _glTextureId;
		uint32_t _width;
		uint32_t _height;
		bool _stbImage;
		bool _hasGlObject;
	};
}
#endif // !PIXELEXPLORER_RENDERING_TEXTURE_H_
