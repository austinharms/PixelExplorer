#ifndef PIXELEXPLORER_APPLICATION_SCENE_H_
#define PIXELEXPLORER_APPLICATION_SCENE_H_
#include <new>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeScene.h"
#include "PxeWindow.h"
#include "Log.h"

namespace pixelexplorer {
	class ApplicationScene : public pxengine::PxeRefCount
	{
	public:
		virtual void onStart(pxengine::PxeWindow& window) = 0;
		virtual void onUpdate() {}
		virtual void onClose(bool& shouldClose) {}
		virtual void onStop() = 0;
		PXE_NODISCARD pxengine::PxeScene& getScene();
		PXE_NODISCARD const pxengine::PxeScene& getScene() const;
		virtual ~ApplicationScene();
		PXE_NOCOPY(ApplicationScene);

	protected:
		static pxengine::PxeScene& createPxeScene();

		ApplicationScene();

	private:
		pxengine::PxeScene& _scene;
	};
}
#endif // !PIXELEXPLORER_APPLICATION_SCENE_H_

