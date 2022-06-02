#include <stdint.h>

#include "RenderObject.h"
#include "RenderWindow.h"
#include "Shader.h"
#include "Material.h"

#ifndef PIXELEXPLORE_RENDERING_RENDERMESH_H_
#define  PIXELEXPLORE_RENDERING_RENDERMESH_H_
namespace pixelexplorer::rendering {
	class RenderMesh : public RenderObject
	{
	public:
		RenderMesh();
		virtual ~RenderMesh();
		Shader* getShader();
		Material* getMaterial();
		void setPosition(const glm::vec3& pos);

	protected:
		void destroyGLObjects(RenderWindow* window);
		void createGLObjects(RenderWindow* window);
		void drawMesh();

	private:
		Shader* _shader;
		Material* _material;
		uint32_t _vertexArrayGlId;
		uint32_t _vertexBufferGlId;
		uint32_t _indexBufferGlId;
	};
}
#endif // !PIXELEXPLORE_RENDERING_RENDERMESH_H_
