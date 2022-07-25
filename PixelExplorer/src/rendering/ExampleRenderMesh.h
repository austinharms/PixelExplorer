#include <stdint.h>

#include "BasicGLRenderObject.h"
#include "Shader.h"
#include "Material.h"
#include "glm/mat4x4.hpp"

#ifndef PIXELEXPLORE_RENDERING_EXAMPLERENDERMESH_H_
#define  PIXELEXPLORE_RENDERING_EXAMPLERENDERMESH_H_
namespace pixelexplorer::rendering {
	class ExampleRenderMesh : public BasicGLRenderObject
	{
	public:
		ExampleRenderMesh();
		virtual ~ExampleRenderMesh();
		void setPosition(const glm::vec3& pos);

	protected:
		void onInitialize() override;
		void onTerminate() override;
		void onRender() override;

	private:
		Shader* _shader;
		Material* _material;
		glm::mat4 _positionMatrix;
		uint32_t _vertexArrayGlId;
		uint32_t _vertexBufferGlId;
		uint32_t _indexBufferGlId;
	};
}
#endif // !PIXELEXPLORE_RENDERING_RENDERMESH_H_
