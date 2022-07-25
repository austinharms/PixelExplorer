#include "GLObject.h"
#include <stdint.h>

#ifndef PIXELEXPLORE_BASIC_GL_RENDER_OBJECT_H_
#define PIXELEXPLORE_BASIC_GL_RENDER_OBJECT_H_
namespace pixelexplorer::rendering {
	class BasicGLRenderObject : public GLObject
	{
		friend class RenderWindow;
	public:
		inline BasicGLRenderObject() {}
		inline virtual ~BasicGLRenderObject() {}
		virtual inline uint16_t getRenderIndex() const { return 0; }

	protected:
		virtual void onRender() = 0;
	};
}
#endif