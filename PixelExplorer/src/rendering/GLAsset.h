#include "RefCount.h"
#include "GLObject.h"

#ifndef PIXELEXPLORER_RENDERING_GLASSET_H_
#define PIXELEXPLORER_RENDERING_GLASSET_H_
namespace pixelexplorer::rendering {
	class GLAsset : public GLObject
	{
	public:
		inline GLAsset() { _updateFlag = false; }
		inline virtual ~GLAsset() {}
		inline bool getUpdateFlag() const { return _updateFlag; }

	protected:
		virtual inline void onUpdate() {}
		virtual inline void setUpdateFlag() { _updateFlag = true; }

	private:
		friend class RenderWindow;
		bool _updateFlag;
		inline void update() { _updateFlag = false; onUpdate(); }
	};
}
#endif // !PIXELEXPLORER_RENDERING_GLASSET_H_
