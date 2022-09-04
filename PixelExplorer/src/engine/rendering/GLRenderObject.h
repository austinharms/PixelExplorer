#include "GLNode.h"
#include "GLObject.h"
#include "common/Logger.h"

#ifndef PIXELEXPLORER_ENGINE_RENDERING_GLRENDEROBJECT_H_
#define PIXELEXPLORER_ENGINE_RENDERING_GLRENDEROBJECT_H_
namespace pixelexplorer::engine::rendering {
	class GLRenderObject : private GLNode<GLRenderObject>, public GLObject
	{
	public:
		inline bool inRenderQueue() const { return GLNode<GLRenderObject>::prev != nullptr; }
		inline bool drop() override {

			bool dropped = GLObject::drop();
			if (!dropped && getRefCount() == 2 && getAttached() && inRenderQueue()) {
				Logger::warn(__FUNCTION__" GLRenderObject may have been abandoned on the render queue, did you forget to remove it from the render queue before dropping it?");
			}

			return dropped;
		}

	private:
		friend class RenderWindow;
	};
}
#endif