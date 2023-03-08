#ifndef PXENGINE_RENDER_TARGET_H_
#define PXENGINE_RENDER_TARGET_H_
#include "PxeTypes.h"
#include "PxeCamera.h"
#include "PxeRenderTexture.h"
#include "PxeScene.h"

namespace pxengine {
	class PxeRenderTarget
	{
	public:
		PxeRenderTarget() = default;
		PXE_NODISCARD virtual PxeCamera* getRenderCamera() const = 0;
		PXE_NODISCARD virtual PxeRenderTexture* getRenderOutputTexture() const = 0;
		PXE_NODISCARD virtual PxeScene* getRenderScene() const = 0;
		PXE_NODISCARD virtual int32_t getRenderWidth() const = 0;
		PXE_NODISCARD virtual int32_t getRenderHeight() const = 0;
		virtual void onPrepareForRender() = 0;
		virtual void onRenderCompleted() = 0;
		PXE_NODISCARD bool getRenderTargetValid() const { return getRenderCamera() && getRenderOutputTexture() && getRenderWidth() > 0 && getRenderHeight() > 0 && getRenderScene(); }
	};
}
#endif // !PXENGINE_RENDER_TARGET_H_
