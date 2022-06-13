#include "RefCount.h"
#include "GLObject.h"

#ifndef PIXELEXPLORER_RENDERING_GLASSET_H_
#define PIXELEXPLORER_RENDERING_GLASSET_H_
namespace pixelexplorer::rendering {
	class GLAsset : public GLObject
	{
	public:
		inline GLAsset() {}
		inline virtual ~GLAsset() {}
		inline bool drop() {
			if (getRefCount() == 2)
				getRenderWindow();

			return RefCount::drop();
		}
	};
}
#endif // !PIXELEXPLORER_RENDERING_GLASSET_H_
