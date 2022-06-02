#include "RefCount.h"

#ifndef PIXELEXPLORER_RENDERING_TEXTURE_H_
#define PIXELEXPLORER_RENDERING_TEXTURE_H_
namespace pixelexplorer::rendering {
	class Texture : public RefCount
	{
	public:
		Texture();
		~Texture();

	private:

	};

	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
	}
}
#endif // !PIXELEXPLORER_RENDERING_TEXTURE_H_
