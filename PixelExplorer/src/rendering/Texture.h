#include <stdint.h>

#include "RefCount.h"
#include "stb_image.h"

#ifndef PIXELEXPLORER_RENDERING_TEXTURE_H_
#define PIXELEXPLORER_RENDERING_TEXTURE_H_
namespace pixelexplorer::rendering {
	class Texture : public RefCount
	{
	public:
		Texture();
		virtual ~Texture();

	private:
		uint8_t* _pixelBuffer;
	};
}
#endif // !PIXELEXPLORER_RENDERING_TEXTURE_H_
