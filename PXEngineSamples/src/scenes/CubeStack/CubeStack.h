#ifndef PXENGINESAMPELS_SCENES_CUBESTACK_H_
#define PXENGINESAMPELS_SCENES_CUBESTACK_H_
#include "PxeEngineAPI.h"
#include "../../SampleSceneBase.h"
#include "PhysicsCube.h"
#include "SDL_keycode.h"

class CubeStack : public SampleSceneBase
{
public:
	CubeStack() {
		using namespace pxengine;
		using namespace cubestack;
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

		PxeShader* shader = engine.loadShader(getAssetPath("shaders") / "test.pxeshader");
		if (!shader) {
			scene->drop();
			_error = true;
			return;
		}

		PxeTexture* texture = new(std::nothrow) PxeTexture();
		if (!texture) {
			_error = true;
			shader->drop();
			scene->drop();
			return;
		}

		texture->loadImage(pxengine::getAssetPath("textures") / "test.png");

		PxeRenderMaterial* material = new(std::nothrow) PxeRenderMaterial(*shader);
		if (!material) {
			_error = true;
			texture->drop();
			shader->drop();
			scene->drop();
			return;
		}

		material->setTexture("u_Texture", *texture, 0);
		material->setProperty4f("u_Color", glm::vec4(1, 1, 1, 1));

		PhysicsCube* baseCube = PhysicsCube::createPhysicsCube(glm::vec3(0, -10, 0), false, *material);
		if (!baseCube) {
			_error = true;
			material->drop();
			texture->drop();
			shader->drop();
			scene->drop();
			return;
		}

		scene->addRenderable(*baseCube);
		for (int32_t y = -9; y < 10; ++y) {
			PhysicsCube* cube = PhysicsCube::createPhysicsCube(glm::vec3(0, y * 1.1f, 0), true, *material, baseCube->getIndexBuffer(), baseCube->getVertexArray(), baseCube->getShape());
			if (!cube) continue;
			scene->addRenderable(*cube);
			cube->drop();
		}

		baseCube->drop();
		material->drop();
		texture->drop();
		shader->drop();
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
