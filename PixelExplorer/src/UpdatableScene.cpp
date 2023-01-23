#include "UpdatableScene.h"

#include "Log.h"
#include "PxeEngine.h"

using namespace pxengine;
namespace pixelexplorer {
	UpdatableScene::UpdatableScene() {
		_scene = pxeGetEngine().createScene();
		if (!_scene) {
			PEX_ERROR("Failed to create PxeScene");
			return;
		}

		_scene->setUserData(this);
	}

	UpdatableScene::~UpdatableScene() {
		if (!_scene->drop()) {
			PEX_WARN("PxeScene not dropped on UpdatableScene destroy");
		}

		_scene = nullptr;
	}

	pxengine::PxeScene* UpdatableScene::getScene() const
	{
		return _scene;
	}
}