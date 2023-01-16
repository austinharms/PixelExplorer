#ifndef PXENGINE_RENDER_BASE_H_
#define PXENGINE_RENDER_BASE_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	// Base class for all renderable objects
	class PxeRenderBase : public PxeRefCount
	{
	public:
		inline PxeRenderBase(PxeRenderPass renderPass) : _renderPass(renderPass) {}
		virtual ~PxeRenderBase() = default;

		// Returns the object required render pass
		inline PXE_NODISCARD PxeRenderPass getRenderPass() const { return _renderPass; }

		// This method should draw the object to the current framebuffer
		virtual void onRender() = 0;

		PxeRenderBase(const PxeRenderBase& other) = delete;
		PxeRenderBase operator=(const PxeRenderBase& other) = delete;

	private:
		const PxeRenderPass _renderPass;
	};
}
#endif // !PXENGINE_RENDER_BASE_H_