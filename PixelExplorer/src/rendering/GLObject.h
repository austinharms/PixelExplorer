#include <assert.h>

#include "RefCount.h"
#include "Logger.h"

#ifndef PIXELEXPLORE_RENDERING_GLOBJECT_H_
#define PIXELEXPLORE_RENDERING_GLOBJECT_H_
namespace pixelexplorer::rendering {
	class GLObject : public RefCount
	{
		friend class RenderWindow;
	public:
		inline GLObject() { _hasGLObjects = false; }
		virtual ~GLObject() {
#ifdef DEBUG
			assert(!_hasGlObjects);
#else
			if (_hasGLObjects)
				Logger::error("GLObject not deallocated");
#endif // DEBUG
		}

	protected:
		inline virtual void createGLObjects(RenderWindow* window) {}
		inline virtual void destroyGLObjects(RenderWindow* window) {}
		inline virtual bool requiresGLObjects() = 0;
		inline void setHasGLObjects(bool value) { _hasGLObjects = value; }
		inline bool getHasGLObjects() const { return _hasGLObjects; }

	private:
		bool _hasGLObjects;
	};
}

#endif // !PIXELEXPLORE_RENDERING_GLOBJECT_H_
