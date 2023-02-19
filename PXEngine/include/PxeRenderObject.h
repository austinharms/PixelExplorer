#ifndef PXENGINE_RENDER_OBJECT_H_
#define PXENGINE_RENDER_OBJECT_H_
#include "PxeTypes.h"
#include "PxeObject.h"
#include "PxeRenderObjectInterface.h"
#include "PxeRenderMaterialInterface.h"

namespace pxengine {
	// base class for rendering geometry in world space
	class PxeRenderObject : public PxeObject, public PxeRenderObjectInterface
	{
	public:
		inline PxeRenderObject(PxeRenderMaterialInterface& material, const glm::mat4& positionMatrix = glm::mat4(1), PxeObjectFlags flags = PxeObjectFlags::RENDER_OBJECT) : PxeObject(flags), PxeRenderObjectInterface(material), positionMatrix(positionMatrix) {}
		virtual ~PxeRenderObject() = default;

		// This method should draw the object to the current framebuffer
		// Note: you can assume a valid OpenGl context
		// Note: this requires the RENDER_OBJECT flag to be set (flag is set by default)
		virtual void onRender() override = 0;

		PXE_NODISCARD const glm::mat4& getPositionMatrix() const override { return positionMatrix; }

	protected:
		glm::mat4 positionMatrix;
	};
}
#endif // !PXENGINE_RENDER_OBJECT_H_
