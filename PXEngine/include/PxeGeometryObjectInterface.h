#ifndef PXENGINE_GEOMETRY_OBJECT_INTERFACE_H
#define PXENGINE_GEOMETRY_OBJECT_INTERFACE_H
#include "PxeTypes.h"
#include "glm/mat4x4.hpp"
#include "PxeRenderMaterial.h"

namespace pxengine {
	// Interface class for rendering 3D geometry using a PxeScene
	class PxeGeometryObjectInterface
	{
	public:
		PxeGeometryObjectInterface(PxeRenderMaterial& material, const glm::mat4& positionMat = glm::mat4(1.0f)) : _renderMaterial(material), positionMatrix(positionMat) { _renderMaterial.grab(); }
		virtual ~PxeGeometryObjectInterface() { _renderMaterial.drop(); }
		inline PXE_NODISCARD PxeRenderMaterial& getRenderMaterial() const { return _renderMaterial; }
		inline PXE_NODISCARD const glm::mat4& getPositionMatrix() const { return positionMatrix; }

		// This method should draw the object to the current framebuffer
		// Note: this requires the GEOMETRY_UPDATE flag to be set on the base PxeObject
		virtual void onGeometry() = 0;

	protected:
		glm::mat4 positionMatrix;

	private:
		PxeRenderMaterial& _renderMaterial;
	};
}
#endif // !PXENGINE_GEOMETRY_OBJECT_INTERFACE_H
