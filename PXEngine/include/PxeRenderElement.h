#include "PxeRenderBase.h"

#ifndef PXENGINE_RENDER_ELEMENT_H_
#define PXENGINE_RENDER_ELEMENT_H_
namespace pxengine {
	class PxeRenderElement : public PxeRenderBase
	{
	public:
		inline PxeRenderElement() : PxeRenderBase(PxeRenderBase::RenderSpace::SCREEN_SPACE) {}
		inline virtual ~PxeRenderElement() = default;
	};
}
#endif // !PXENGINE_RENDER_ELEMENT_H_
