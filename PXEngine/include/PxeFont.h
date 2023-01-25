#ifndef PXENGINE_FONT_H_
#define PXENGINE_FONT_H_
#include <filesystem>

#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	// Helper class to manage font scaling 
	class PxeFont : public PxeRefCount
	{
	public:
		PxeFont() = default;
		virtual ~PxeFont() = default;

		// Make the font current in the gui library
		// Note: requires valid gui context
		virtual void guiPushFont() = 0;

		// Make the font not current in the gui library
		// Note: this MUST be called after calling guiPushFont()
		// Note: requires valid gui context
		virtual void guiPopFont() = 0;

		// Returns the size used to create this font instance
		virtual PXE_NODISCARD uint16_t getPixelSize() const = 0;

		// Returns the actual size of the font based on the PxeFontScalingMode and the current gui context window size
		// Note: requires valid gui context
		virtual PXE_NODISCARD uint16_t getScaledPixelSize() const = 0;

		// Returns the path used to load the font
		virtual PXE_NODISCARD const std::filesystem::path& getFontSource() const = 0;

		// Returns the PxeFontScalingMode used to create the font
		virtual PXE_NODISCARD PxeFontScalingMode getScalingMode() const = 0;

		// Returns the reference window size used to create the font
		virtual PXE_NODISCARD const glm::i32vec2& getReferenceWindowSize() const = 0;

		// Returns the current status of the font
		virtual PXE_NODISCARD PxeFontStatus getStatus() const = 0;
	};
}
#endif // !PXENGINE_FONT_H_
