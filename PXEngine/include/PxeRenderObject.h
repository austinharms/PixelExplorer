#include "PxeRenderBase.h"

#ifndef PXENGINE_RENDER_OBJECT_H_
#define PXENGINE_RENDER_OBJECT_H_
namespace pxengine {
	class PxeRenderObject : public PxeRenderBase
	{
	public:
		inline PxeRenderObject() : PxeRenderBase(PxeRenderBase::RenderSpace::WORLD_SPACE) {}
		inline virtual ~PxeRenderObject() = default;
	};
}
#endif // !PXENGINE_RENDER_OBJECT_H_
