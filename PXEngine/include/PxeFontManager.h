#ifndef PXENGINE_FONT_MANAGER_H_
#define PXENGINE_FONT_MANAGER_H_
#include <filesystem>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeFont.h"

namespace pxengine {
	// Helper class to manage font loading 
	class PxeFontManager
	{
	public:
		PxeFontManager() = default;
		virtual ~PxeFontManager() = default;

		// Loads a font file from {path} and returns a new PxeFont instance or nullptr if there was an error
		// Note: this only supports loading .ttf font files
		virtual PXE_NODISCARD PxeFont* loadFont(const std::filesystem::path& path, uint16_t size, PxeFontScalingMode scaleMode = PxeFontScalingMode::SMALLEST, const glm::i32vec2& referenceWindowSize = glm::i32vec2(600, 400)) = 0;
	};
}
#endif // !PXENGINE_FONT_MANAGER_H_