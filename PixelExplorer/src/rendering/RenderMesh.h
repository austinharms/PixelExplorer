#include <stdint.h>

#include "RenderObject.h"
#include "RenderWindow.h"
#include "Shader.h"
#include "glm/mat4x4.hpp"

#ifndef PIXELEXPLORE_RENDERING_RENDERMESH_H_
#define  PIXELEXPLORE_RENDERING_RENDERMESH_H_
namespace pixelexplore::rendering {
	class RenderMesh : public RenderObject
	{
	public:
		RenderMesh();
		virtual ~RenderMesh();
		Shader* getShader();

	protected:
		void deleteGlObjects(RenderWindow* window);
		void createGlObjects(RenderWindow* window);
		void drawMesh();

	private:
		Shader* _shader;
		uint32_t _vertexArrayGlId;
		uint32_t _vertexBufferGlId;
		uint32_t _indexBufferGlId;
	};
}
#endif // !PIXELEXPLORE_RENDERING_RENDERMESH_H_
