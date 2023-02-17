#ifndef PXENGINE_GEOMETRY_OBJECT_INTERFACE_H
#define PXENGINE_GEOMETRY_OBJECT_INTERFACE_H
#include "PxeTypes.h"
#include "glm/mat4x4.hpp"
#include "PxeRenderMaterialInterface.h"

namespace pxengine {
	// Interface class for rendering 3D geometry using a PxeScene
	class PxeRenderObjectInterface
	{
	public:
		PxeRenderObjectInterface(PxeRenderMaterialInterface& material) : _renderMaterial(material) { _renderMaterial.grab(); }
		virtual ~PxeRenderObjectInterface() { _renderMaterial.drop(); }

		// Return the position matrix
		// Note: this matrix is used in the creation of the MVP matrix that is bound when the object is rendered
		// Note: this is not used in the GUI render pass
		PXE_NODISCARD virtual const glm::mat4& getPositionMatrix() const = 0;

		// This method should draw the object to the current framebuffer
		// Note: this requires the RENDER_OBJECT flag to be set on the base PxeObject
		virtual void onRender() = 0;

		PXE_NODISCARD inline PxeRenderMaterialInterface& getRenderMaterial() const { return _renderMaterial; }

	private:
		PxeRenderMaterialInterface& _renderMaterial;
	};
}
#endif // !PXENGINE_GEOMETRY_OBJECT_INTERFACE_H
