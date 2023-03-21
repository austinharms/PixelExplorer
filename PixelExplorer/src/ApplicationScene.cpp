#include "ApplicationScene.h"

#include "PxeEngine.h"

namespace pixelexplorer {
	pxengine::PxeScene* ApplicationScene::createPxeScene()
	{
		return pxengine::PxeEngine::getInstance().createScene();
	}

	ApplicationScene::ApplicationScene(pxengine::PxeScene& scene) : _scene(scene)
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
