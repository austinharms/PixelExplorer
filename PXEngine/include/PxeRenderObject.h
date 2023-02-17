#ifndef PXENGINE_RENDER_OBJECT_H_
#define PXENGINE_RENDER_OBJECT_H_
#include "PxeTypes.h"
#include "PxeObject.h"
#include "PxeRenderObjectInterface.h"

namespace pxengine {
	// base class for rendering geometry in world space
	class PxeRenderObject : public PxeObject, public PxeRenderObjectInterface
	{
	public:
		inline PxeRenderObject(PxeRenderMaterial& material, const glm::mat4& positionMatrix = glm::mat4(1), PxeObjectFlags flags = PxeObjectFlags::RENDER_OBJECT) : PxeObject(flags), PxeRenderObjectInterface(material) {}
		virtual ~PxeRenderObject() = default;

		// This method should draw the object to the current framebuffer
		// Note: you can assume a valid OpenGl context
		// Note: this requires the RENDER_OBJECT flag to be set (flag is set by default)
		virtual void onRender() override = 0;
	};
}
#endif // !PXENGINE_RENDER_OBJECT_H_
