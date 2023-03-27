#include "ApplicationScene.h"

#include "PxeEngine.h"
#include "Log.h"

namespace pixelexplorer {
	pxengine::PxeScene& ApplicationScene::createPxeScene()
	{
		pxengine::PxeScene* scene = pxengine::PxeEngine::getInstance().createScene();
		if (!scene) {
			PEX_FATAL("Failed to create PxeScene");
		}

		return *scene;
	}

	ApplicationScene::ApplicationScene() : _scene(createPxeScene())
	{
		_scene.userData = this;
	}

	ApplicationScene::~ApplicationScene()
	{
		if (!_scene.drop()) {
			PEX_WARN("PxeScene not destroyed on ApplicationScene destroy");
		}
	}

	PXE_NODISCARD pxengine::PxeScene& ApplicationScene::getScene()
	{
		return _scene;
	}

	PXE_NODISCARD const pxengine::PxeScene& ApplicationScene::getScene() const
	{
		return _scene;
	}
}
