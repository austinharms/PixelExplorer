#include <stdint.h>
#include <filesystem>

#include "common/RefCount.h"
#include "GLObject.h"
#include "stb_image.h"

#ifndef PIXELEXPLORER_ENGINE_RENDERING_GLTEXTURE_H_
#define PIXELEXPLORER_ENGINE_RENDERING_GLTEXTURE_H_
namespace pixelexplorer::engine::rendering {
	class GLTexture : public GLObject
	{
	public:
		GLTexture(uint32_t width, uint32_t height);
		GLTexture(const std::filesystem::path& path);
		virtual ~GLTexture();
		void bind(uint8_t slot = 0);
		inline uint32_t getGlId() const { return _glTextureId; }
		inline uint32_t getWidth() const { return _width; }
		inline uint32_t getHeight() const { return _height; }

	protected:
		void onInitialize() override;
		void onTerminate() override;
		void onUpdate() override;

	private:
		uint8_t* _pixelBuffer;
		uint32_t _glTextureId;
		uint32_t _width;
		uint32_t _height;
		bool _stbImage;
	};
}
#endif // !PIXELEXPLORER_RENDERING_TEXTURE_H_
