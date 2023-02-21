#ifndef PXENGINESAMPELS_SCENES_CUBESTACK_H_
#define PXENGINESAMPELS_SCENES_CUBESTACK_H_
#include "PxeEngineAPI.h"
#include "../../SampleSceneBase.h"
#include "PhysicsCube.hpp"
#include "SDL_keycode.h"

class CubeStack : public SampleSceneBase
{
public:
	CubeStack() {
		using namespace pxengine;
		_error = false;
		_window = nullptr;
		_quitAction = nullptr;

		PxeEngine& engine = pxeGetEngine();
		PxeInputManager& inputMgr = engine.getInputManager();

		_window = engine.createWindow(600, 400, "Cube Wall Sample");
		if (!_window) {
			_error = true;
			return;
		}

		_quitAction = inputMgr.getAction("Quit Sample");
		if (!_quitAction) {
			_error = true;
			return;
		}

		_quitAction->grab();
		if (!_quitAction->hasSource()) {
			PxeActionSource* quitSource = inputMgr.getActionSource(PxeKeyboardActionSourceCode(SDLK_ESCAPE));
			if (!quitSource) {
				_error = true;
				return;
			}

			_quitAction->addSource(*quitSource);
		}

		pxengine::PxeScene* scene = pxeGetEngine().createScene();
		if (!scene) {
			_error = true;
			return;
		}

		_window->setScene(scene);
		PxeUnlitRenderMaterial* cubeMaterial = PxeUnlitRenderMaterial::createMaterial();
		if (!cubeMaterial) {
			_error = true;
			scene->drop();
			return;
		}

		{
			PxeTexture* texture = new(std::nothrow) PxeTexture();
			if (texture) {
				texture->loadImage(pxengine::getAssetPath("textures") / "cube.png");
				cubeMaterial->setTexture(texture);
				texture->drop();
			}
		}

		{
			PhysicsCube* baseCube = PhysicsCube::createPhysicsCube(glm::vec3(0, -9, 0), false, *cubeMaterial);
			if (!baseCube) {
				_error = true;
				cubeMaterial->drop();
				scene->drop();
				return;
			}

			scene->addObject(*baseCube);
			baseCube->drop();
		}

		for (int32_t y = -8; y < 10; ++y) {
			PhysicsCube* cube = PhysicsCube::createPhysicsCube(glm::vec3(0, y * 1.05f, 0), true, *cubeMaterial);
			if (!cube) continue;
			scene->addObject(*cube);
			cube->drop();
		}

		cubeMaterial->drop();
		scene->drop();
	}

	bool update() override {
		if (_error || _window->getShouldClose()) return false;
		if (pxengine::pxeGetEngine().getInputManager().getKeyboardFocusedWindow() == _window && _quitAction->getValue()) return false;
		return true;
	}

	virtual ~CubeStack() {
		if (_window) {
			_window->drop();
			_window = nullptr;
		}

		if (_quitAction) {
			_quitAction->drop();
			_quitAction = nullptr;
		}
	}

private:
	pxengine::PxeWindow* _window;
	pxengine::PxeAction* _quitAction;
	bool _error;
};
#endif // !PXENGINESAMPELS_SCENES_CUBESTACK_H_
