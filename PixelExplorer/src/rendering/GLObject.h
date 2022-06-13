#include <assert.h>

#include "GLObject.fwd.h"
#include "RenderWindow.fwd.h"
#include "RefCount.h"

#ifndef PIXELEXPLORE_RENDERING_GLOBJECT_H_
#define PIXELEXPLORE_RENDERING_GLOBJECT_H_
namespace pixelexplorer::rendering {
	class GLObject : public RefCount
	{
	public:
		GLObject();
		virtual ~GLObject();

	protected:
		RenderWindow* getRenderWindow();
		virtual void createGLObjects() = 0;
		virtual void destroyGLObjects() = 0;
		virtual bool requiresGLObjects() = 0;

	private:
		friend class RenderWindow;
		RenderWindow* _currentWindow;
		bool _objectInitialized;

		void initGLObjects();
		void uninitGLObjects();
	};
}

#endif // !PIXELEXPLORE_RENDERING_GLOBJECT_H_
