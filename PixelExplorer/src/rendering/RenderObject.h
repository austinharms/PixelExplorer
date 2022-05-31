#include <assert.h>

#include "RefCount.h"
#include "glm/mat4x4.hpp"
#include "Shader.h"
#include "Logger.h"

#ifndef PIXELEXPLORE_RENDERING_RENDEROBJECT_H_
#define PIXELEXPLORE_RENDERING_RENDEROBJECT_H_
namespace pixelexplore::rendering {
	class RenderObject : public RefCount
	{
		friend class RenderWindow;
	public:
		inline RenderObject() {
			positionMatrix = glm::mat4(1);
			_hasGlObjects = false;
		}

		inline virtual ~RenderObject() {
#ifdef DEBUG
			assert(!_hasGlObjects);
#else
			if (_hasGlObjects)
				Logger::error("Render Object GL Buffers not deallocated");
#endif // DEBUG
		}

		virtual Shader* getShader() = 0;
		inline virtual const glm::mat4 getPositionMatrix() const { return positionMatrix; }

	protected:
		glm::mat4 positionMatrix;

		inline void setHasGlObjects(bool value) { _hasGlObjects = value; }
		inline bool getHasGlObjects() const { return _hasGlObjects; }
		inline virtual bool meshVisible() { return true; }
		inline virtual glm::mat4 getPositionMatrix() { return positionMatrix; }
		virtual void deleteGlObjects(RenderWindow* window) = 0;
		virtual void createGlObjects(RenderWindow* window) = 0;
		virtual void drawMesh() = 0;

	private:
		bool _hasGlObjects;
	};
}
#endif // !PIXELEXPLORE_RENDERING_RENDEROBJECT_H_

