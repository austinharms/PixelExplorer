#ifndef PXENGINESAMPELS_SCENES_CUBEWALL_H_
#define PXENGINESAMPELS_SCENES_CUBEWALL_H_
#include <cstdlib>
#include <ctime>

#include "PxeEngineAPI.h"
#include "../../SampleSceneBase.h"
#include "RenderCube.h"
#include "SDL_keycode.h"

class CubeWall : public SampleSceneBase
{
public:
	CubeWall() {
		using namespace pxengine;
		using namespace cubewall;
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

		RenderCube* baseCube = new(std::nothrow) RenderCube(*material);
		if (!baseCube) {
			_error = true;
			material->drop();
			texture->drop();
			shader->drop();
			scene->drop();
			return;
		}

		srand(static_cast<uint32_t>(time(nullptr)));
		for (int32_t x = -25; x < 26; ++x) {
			for (int32_t y = -25; y < 26; ++y) {
				RenderCube* cube = new(std::nothrow) RenderCube(*material, baseCube->getIndexBuffer(), baseCube->getVertexArray());
				if (!cube) continue;
				cube->translate(glm::vec3(x, y, 0));
				cube->rotateAbout(glm::vec3(1, 0, 0), ((float)rand() / (float)RAND_MAX) * glm::two_pi<float>());
				cube->rotateAbout(glm::vec3(0, 1, 0), ((float)rand() / (float)RAND_MAX) * glm::two_pi<float>());
				cube->rotateAbout(glm::vec3(0, 0, 1), ((float)rand() / (float)RAND_MAX) * glm::two_pi<float>());
				scene->addObject(*cube);
				cube->drop();
			}
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
