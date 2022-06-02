#include "GLObject.h"
#include "glm/mat4x4.hpp"
#include "Shader.h"
#include "Logger.h"
#include "Material.h"

#ifndef PIXELEXPLORE_RENDERING_RENDEROBJECT_H_
#define PIXELEXPLORE_RENDERING_RENDEROBJECT_H_
namespace pixelexplorer::rendering {
	class RenderObject : public GLObject
	{
		friend class RenderWindow;
	public:
		inline RenderObject() { positionMatrix = glm::mat4(1); }
		inline virtual ~RenderObject() { }
		virtual Shader* getShader() = 0;
		inline virtual Material* getMaterial() { return nullptr; }
		inline virtual const glm::mat4 getPositionMatrix() const { return positionMatrix; }

	protected:
		glm::mat4 positionMatrix;

		inline virtual bool meshVisible() { return true; }
		inline virtual glm::mat4 getPositionMatrix() { return positionMatrix; }
		inline bool requiresGLObjects() { return true; }
		virtual void drawMesh() = 0;
	};
}
#endif // !PIXELEXPLORE_RENDERING_RENDEROBJECT_H_

