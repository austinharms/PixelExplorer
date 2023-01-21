#ifndef PXENGINE_RENDER_ELEMENT_H_
#define PXENGINE_RENDER_ELEMENT_H_
#include "PxeTypes.h"
#include "PxeRenderBase.h"

namespace pxengine {
	// base class for GUI rendering
	class PxeRenderElement : public PxeRenderBase
	{
	public:
		inline PxeRenderElement() : PxeRenderBase(PxeRenderPass::SCREEN_SPACE) {}
		virtual ~PxeRenderElement() = default;

		// This method should draw the object to the current framebuffer
		// Note: you can assume a valid ImGui context and frame
		virtual void onRender() override = 0;
	};
}
#endif // !PXENGINE_RENDER_ELEMENT_H_
