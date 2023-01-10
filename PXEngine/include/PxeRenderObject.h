#include "PxeRenderBase.h"
#include "PxeRenderMaterial.h"
#include "glm/mat4x4.hpp"

#ifndef PXENGINE_RENDER_OBJECT_H_
#define PXENGINE_RENDER_OBJECT_H_
namespace pxengine {
	class PxeRenderObject : public PxeRenderBase
	{
	public:
		inline PxeRenderObject(PxeRenderMaterial& material) : PxeRenderBase(PxeRenderBase::RenderSpace::WORLD_SPACE), _renderMaterial(material), positionMatrix(1.0f) { _renderMaterial.grab(); }
		inline virtual ~PxeRenderObject() { _renderMaterial.drop(); }
		inline PxeRenderMaterial& getRenderMaterial() const { return _renderMaterial; }
		inline const glm::mat4& getPositionMatrix() const { return positionMatrix; }

	protected:
		glm::mat3 positionMatrix;

	private:
		PxeRenderMaterial& _renderMaterial;
	};
}
#endif // !PXENGINE_RENDER_OBJECT_H_
