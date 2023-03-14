#ifndef PXENGINESAMPELS_SCENES_FREE_CAMERA_H_
#define PXENGINESAMPELS_SCENES_FREE_CAMERA_H_
#include <cassert>

#include "PxeEngineAPI.h"
#include "SampleSceneBase.h"
#include "shared/PxesRenderData.h"
#include "shared/PxesRenderDataComponent.h"
#include "shared/PxesBasicRenderProperties.h"
#include "Camera.h"
#include "SDL_keycode.h"

class FreeCameraSample : public SampleSceneBase
{
public:
	FreeCameraSample() {
		using namespace pxengine;
		_error = false;
		_window = nullptr;
		_quitAction = nullptr;
		_camera = nullptr;

		PxeEngine& engine = PxeEngine::getInstance();
		PxeInputManager& inputMgr = engine.getInputManager();
		_window = engine.createWindow(600, 400, "Free Camera Sample");
		if (!_window) {
			_error = true;
			return;
		}

		_quitAction = inputMgr.getAction("Quit Sample");
		if (!_quitAction) {
			_error = true;
			return;
		}

		if (!_quitAction->hasSource()) {
			PxeActionSource* quitSource = inputMgr.getActionSource(PxeKeyboardActionSourceCode(SDLK_ESCAPE));
			if (!quitSource) {
				_error = true;
				return;
			}

			_quitAction->addSource(*quitSource);
			quitSource->drop();
		}

		pxengine::PxeScene* scene = engine.createScene();
		if (!scene) {
			_error = true;
			return;
		}

		_window->setScene(scene);

		PxeTexture* texture = new PxeTexture();
		texture->loadImage(pxengine::getAssetPath("textures") / "cube.png");
		PxesBasicRenderProperties* renderProperties = new PxesBasicRenderProperties(*texture);
		texture->drop();

		PxeObject* cubeObject = PxeObject::create();
		if (!cubeObject) {
			renderProperties->drop();
			_error = true;
			return;
		}

		PxesRenderData* cubeRenderData = new PxesRenderData();
		PxesRenderDataComponent* renderComponent = new PxesRenderDataComponent(*cubeRenderData, *renderProperties);
		cubeRenderData->drop();
		renderProperties->drop();
		assert(cubeObject->addComponent(*renderComponent));
		renderComponent->drop();
		assert(scene->addObject(*cubeObject));
		cubeObject->drop();
		scene->drop();
		_camera = new Camera(*_window, glm::radians(90.0f));
		_camera->setPosition(glm::vec3(0, 0, -10));
	}

	bool update() override {
		if (_error || _window->getShouldClose()) return false;
		if (pxengine::PxeEngine::getInstance().getInputManager().getKeyboardFocusedWindow() == _window && _quitAction->getValue()) return false;
		_camera->update();
		return true;
	}

	virtual ~FreeCameraSample() {
		if (_window) {
			_window->drop();
			_window = nullptr;
		}

		if (_quitAction) {
			_quitAction->drop();
			_quitAction = nullptr;
		}

		if (_camera) {
			_camera->unlockCursor();
			_camera->drop();
			_camera = nullptr;
		}
	}

private:
	pxengine::PxeWindow* _window;
	pxengine::PxeAction* _quitAction;
	Camera* _camera;
	bool _error;
};
#endif // !PXENGINESAMPELS_SCENES_CUBEWALL_H_
