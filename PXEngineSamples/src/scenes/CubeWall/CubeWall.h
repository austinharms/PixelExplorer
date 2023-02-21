#ifndef PXENGINESAMPELS_SCENES_CUBEWALL_H_
#define PXENGINESAMPELS_SCENES_CUBEWALL_H_
#include <cstdlib>
#include <ctime>

#include "PxeEngineAPI.h"
#include "../../SampleSceneBase.h"
#include "../shared/RenderCube.hpp"
#include "SDL_keycode.h"

class CubeWall : public SampleSceneBase
{
public:
	CubeWall() {
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
			PxeActionSource* quitSource = inputMgr.getActionSource((PxeActionSourceCode)PxeActionSourceType::KEYBOARD << 32 | SDLK_ESCAPE);
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

		srand(static_cast<uint32_t>(time(nullptr)));
		for (int32_t x = -10; x < 11; ++x) {
			for (int32_t y = -10; y < 11; ++y) {
				RenderCube* cube = RenderCube::create(*cubeMaterial);
				cube->translate(glm::vec3(x * 2, y * 2, 0));
				cube->rotateAbout(glm::vec3(1, 0, 0), ((float)rand() / (float)RAND_MAX) * glm::two_pi<float>());
				cube->rotateAbout(glm::vec3(0, 1, 0), ((float)rand() / (float)RAND_MAX) * glm::two_pi<float>());
				cube->rotateAbout(glm::vec3(0, 0, 1), ((float)rand() / (float)RAND_MAX) * glm::two_pi<float>());
				scene->addObject(*cube);
				cube->drop();
			}
		}

		cubeMaterial->drop();
		scene->drop();
	}

	bool update() override {
		if (_error || _window->getShouldClose()) return false;
		if (pxengine::pxeGetEngine().getInputManager().getKeyboardFocusedWindow() == _window && _quitAction->getValue()) return false;
		return true;
	}

	virtual ~CubeWall() {
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
#endif // !PXENGINESAMPELS_SCENES_CUBEWALL_H_
