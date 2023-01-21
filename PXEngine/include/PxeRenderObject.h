#ifndef PXENGINE_RENDER_OBJECT_H_
#define PXENGINE_RENDER_OBJECT_H_
#include "PxeTypes.h"
#include "PxeRenderBase.h"
#include "PxeRenderMaterial.h"
#include "glm/mat4x4.hpp"

namespace pxengine {
	// base class for rendering 3D object in world space
	class PxeRenderObject : public PxeRenderBase
	{
	public:
		inline PxeRenderObject(PxeRenderMaterial& material) : PxeRenderBase(PxeRenderPass::WORLD_SPACE), _renderMaterial(material), positionMatrix(1.0f) { _renderMaterial.grab(); }
		virtual ~PxeRenderObject() { _renderMaterial.drop(); }
		inline PXE_NODISCARD PxeRenderMaterial& getRenderMaterial() const { return _renderMaterial; }
		PXE_NODISCARD const glm::mat4& getPositionMatrix() const { return positionMatrix; }

		// This method should draw the object to the current framebuffer
		// Note: you can assume a valid OpenGl context
		virtual void onRender() override = 0;

	protected:
		inline PxeRenderObject(PxeRenderMaterial& material, PxeRenderPass pass) : PxeRenderBase(pass), _renderMaterial(material), positionMatrix(1.0f) { _renderMaterial.grab(); }
		glm::mat4 positionMatrix;

	private:
		PxeRenderMaterial& _renderMaterial;
	};
}
#endif // !PXENGINE_RENDER_OBJECT_H_
