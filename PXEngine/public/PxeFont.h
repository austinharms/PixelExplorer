#ifndef PXENGINE_FONT_H_
#define PXENGINE_FONT_H_
#include <filesystem>

#include "PxeTypes.h"
#include "PxeRefCount.h"
PXE_PRIVATE_IMPLEMENTATION_START
#include "PxeFontFile.h"
PXE_PRIVATE_IMPLEMENTATION_END

namespace pxengine {
	class PxeRenderPipeline;

	// Helper class to manage font loading and scaling base on display size
	class PxeFont : public PxeRefCount
	{
	public:
		// Make the font current in the gui library
		// Note: requires valid gui context
		void pushFont();

		// Make the font not current in the gui library
		// Note: this MUST be called after calling guiPushFont()
		// Note: requires valid gui context
		void popFont();

		// Returns the size used to create this font instance
		PXE_NODISCARD uint16_t getPixelSize() const;

		// Returns the actual size of the font based on the PxeFontScalingMode and the current gui context window size
		// Note: requires valid gui context
		PXE_NODISCARD uint16_t getScaledPixelSize() const;

		// Returns the path used to load the font
		PXE_NODISCARD const std::filesystem::path& getPath() const;

		// Returns the PxeFontScalingMode used to create the font
		PXE_NODISCARD PxeFontScalingMode getScalingMode() const;

		// Returns the reference window size used to create the font
		PXE_NODISCARD const glm::i32vec2& getReferenceWindowSize() const;

		// Returns the current status of the font
		PXE_NODISCARD PxeFontStatus getStatus() const;

		virtual ~PxeFont();
		PXE_NOCOPY(PxeFont);

	private:
		PxeFont();
		static constexpr PxeSize STORAGE_SIZE = 32;
		static constexpr PxeSize STORAGE_ALIGN = 8;
		std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
		friend class PxeRenderPipeline;
		struct Impl;
		PxeFont(PxeFontFile& fontFile, uint16_t size, PxeFontScalingMode mode, const glm::i32vec2& windowRef);
		inline Impl& impl() { return reinterpret_cast<Impl&>(_storage); }
		inline const Impl& impl() const { return reinterpret_cast<const Impl&>(_storage); }
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_FONT_H_
