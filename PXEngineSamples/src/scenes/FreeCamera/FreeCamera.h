#ifndef PXENGINESAMPELS_SCENES_FREE_CAMERA_H_
#define PXENGINESAMPELS_SCENES_FREE_CAMERA_H_
#include "PxeEngineAPI.h"
#include "../../SampleSceneBase.h"
#include "RenderCube.h"
#include "Camera.h"
#include "SDL_keycode.h"

class FreeCamera : public SampleSceneBase
{
public:
	FreeCamera() {
		using namespace pxengine;
		using namespace freecamera;
		_error = false;
		_window = nullptr;
		_quitAction = nullptr;
		_camera = nullptr;

		PxeEngine& engine = pxeGetEngine();
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

		PxeShader* shader = engine.getRenderPipeline().loadShader(getAssetPath("shaders") / "test.pxeshader");
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

		PxeRenderMaterial* material = new(std::nothrow) PxeRenderMaterial(*shader, PxeRenderPass::FORWARD);
		if (!material) {
			_error = true;
			texture->drop();
			shader->drop();
			scene->drop();
			return;
		}

		material->setTexture("u_Texture", *texture, 0);
		material->setProperty4f("u_Color", glm::vec4(1, 1, 1, 1));

		RenderCube* cube = new(std::nothrow) RenderCube(*material);
		if (!cube) {
			_error = true;
			material->drop();
			texture->drop();
			shader->drop();
			scene->drop();
			return;
		}

		scene->addObject(*cube);
		_camera = new(std::nothrow) Camera(*_window, glm::radians(90.0f));
		if (!_camera || _camera->getErrorFlag()) {
			_error = true;
			cube->drop();
			material->drop();
			texture->drop();
			shader->drop();
			scene->drop();
			return;
		}

		_camera->setPosition(glm::vec3(0, 0, -10));
		cube->drop();
		material->drop();
		texture->drop();
		shader->drop();
		scene->drop();
	}

	bool update() override {
		if (_error || _window->getShouldClose()) return false;
		if (pxengine::pxeGetEngine().getInputManager().getKeyboardFocusedWindow() == _window && _quitAction->getValue()) return false;
		_camera->update();
		return true;
	}

	virtual ~FreeCamera() {
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
	freecamera::Camera* _camera;
	bool _error;
};
#endif // !PXENGINESAMPELS_SCENES_CUBEWALL_H_
