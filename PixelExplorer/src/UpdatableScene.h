#ifndef PIXELEXPLORER_UPDATABLE_SCENE_H_
#define PIXELEXPLORER_UPDATABLE_SCENE_H_
#include "PxeScene.h"
#include "PxeRefCount.h"

namespace pixelexplorer {
	class UpdatableScene : public pxengine::PxeRefCount
	{
	public:
		UpdatableScene();
		virtual ~UpdatableScene();
		virtual void update() = 0;
		virtual void quit(bool& shouldQuit) {}
		pxengine::PxeScene* getScene() const;

	private:
		pxengine::PxeScene* _scene;
	};
}
#endif // !PIXELEXPLORER_UPDATABLE_SCENE_H_

