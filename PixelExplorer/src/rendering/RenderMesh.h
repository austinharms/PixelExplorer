#include <stdint.h>

#include "RefCount.h"
#include "Shader.h"
#include "glm/mat4x4.hpp"

#ifndef PIXELEXPLORE_RENDERING_RENDERMESH_H_
#define  PIXELEXPLORE_RENDERING_RENDERMESH_H_
namespace pixelexplore::rendering {
	class RenderMesh : public RefCount
	{
		friend class RenderWindow;
	public:
		RenderMesh();
		virtual ~RenderMesh();
		virtual Shader* getShader();
		virtual void onPreRender();
		virtual inline const glm::mat4 getPositionMatrix() const { return positionMatrix; }

	protected:
		glm::mat4 positionMatrix;

	private:
		uint32_t _vertexArrayGlId;
		uint32_t _vertexBufferGlId;
		uint32_t _indexBufferGlId;
	};
}
#endif // !PIXELEXPLORE_RENDERING_RENDERMESH_H_
