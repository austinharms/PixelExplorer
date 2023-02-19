#ifndef PXENGINE_RENDER_PIPELINE_H_
#define PXENGINE_RENDER_PIPELINE_H_
#include <filesystem>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeShader.h"
#include "PxeFont.h"
#include "PxeRenderMaterialInterface.h"

namespace pxengine {
	// TODO Comment file
	class PxeRenderPipeline : public PxeRefCount
	{
	public:
		PxeRenderPipeline() = default;
		virtual ~PxeRenderPipeline() = default;

		// Loads and returns PxeShader or nullptr on failure 
		// Note: returned shaders still need to be validated before use
		// Note: all PxeShader are cached by {path} and are not guaranteed to be reloaded from disk
		PXE_NODISCARD virtual PxeShader* loadShader(const std::filesystem::path& path) = 0;

		// Loads a font file from {path} and returns a new PxeFont instance or nullptr on failure
		// Note: this only supports loading .ttf font files
		PXE_NODISCARD virtual PxeFont* loadFont(const std::filesystem::path& path, uint16_t baseSize, PxeFontScalingMode scaleMode = PxeFontScalingMode::SMALLEST, const glm::i32vec2& referenceWindowSize = glm::i32vec2(600, 400)) = 0;

		// Set VSync mode for all windows
		// Modes:
		//	0 for immediate updates
		//	1 for updates synchronized with
		//	-1 for adaptive VSync
		// Note: the default is 0
		virtual void setVSyncMode(int8_t mode) = 0;

		// Returns VSync mode for all windows
		PXE_NODISCARD virtual int8_t getVSyncMode() const = 0;

		// Returns the PxeRenderMaterialInterface required to be used to render gui elements
		// Note: this will not be created until the primary window has been created
		PXE_NODISCARD virtual PxeRenderMaterialInterface* getGuiRenderMaterial() const = 0;
	};
}

#endif // !PXENGINE_RENDER_PIPELINE_H_
