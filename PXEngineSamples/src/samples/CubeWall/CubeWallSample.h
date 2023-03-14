#ifndef PXENGINESAMPELS_SCENES_CUBEWALL_H_
#define PXENGINESAMPELS_SCENES_CUBEWALL_H_
#include <cstdlib>
#include <ctime>
#include <cassert>

#include "PxeEngineAPI.h"
#include "SampleSceneBase.h"
#include "shared/PxesRenderDataComponent.h"
#include "shared/PxesRenderData.h"
#include "shared/PxesBasicRenderProperties.h"
#include "SDL_keycode.h"

class CubeWallSample : public SampleSceneBase
{
public:
	CubeWallSample() {
		using namespace pxengine;
		_window = nullptr;
		_quitAction = nullptr;

		PxeEngine& engine = PxeEngine::getInstance();
		PxeInputManager& inputMgr = engine.getInputManager();

		_window = engine.createWindow(600, 400, "Cube Wall Sample");
		assert(_window);

		_quitAction = inputMgr.getAction("Quit Sample");
		assert(_quitAction);

		if (!_quitAction->hasSource()) {
			PxeActionSource* quitSource = inputMgr.getActionSource(PxeKeyboardActionSourceCode(SDLK_ESCAPE));
			assert(quitSource);
			_quitAction->addSource(*quitSource);
			quitSource->drop();
		}

		pxengine::PxeScene* scene = engine.createScene();
		assert(scene);
		_window->setScene(scene);
		
		PxeTexture* texture = new PxeTexture();
		texture->loadImage(pxengine::getAssetPath("textures") / "cube.png");
		PxesBasicRenderProperties* renderProperties = new PxesBasicRenderProperties(*texture);
		texture->drop();
		PxesRenderData* cubeRenderData = new PxesRenderData();
		srand(static_cast<uint32_t>(time(nullptr)));
		for (int32_t x = -10; x < 11; ++x) {
			for (int32_t y = -10; y < 11; ++y) {
				PxeObject* cubeObject = PxeObject::create();
				assert(cubeObject);
				glm::mat4 t = glm::rotate(glm::rotate(glm::rotate(
					glm::translate(glm::mat4(1.0f), glm::vec3(x * 2, y * 2, 0)),
					((float)rand() / (float)RAND_MAX) * glm::two_pi<float>(),
					glm::vec3(1, 0, 0)),
					((float)rand() / (float)RAND_MAX) * glm::two_pi<float>(),
					glm::vec3(0, 1, 0)),
					((float)rand() / (float)RAND_MAX) * glm::two_pi<float>(),
					glm::vec3(0, 0, 1));
				cubeObject->setTransform(t);
				PxesRenderDataComponent* renderComponent = new PxesRenderDataComponent(*cubeRenderData, *renderProperties);
				assert(cubeObject->addComponent(*renderComponent));
				renderComponent->drop();
				assert(scene->addObject(*cubeObject));
				cubeObject->drop();
			}
		}

		cubeRenderData->drop();
		renderProperties->drop();
		scene->drop();
	}

	virtual ~CubeWallSample() {
		if (_window) {
			_window->drop();
			_window = nullptr;
		}

		if (_quitAction) {
			_quitAction->drop();
			_quitAction = nullptr;
		}
	}

	bool update() override {
		if (_window->getShouldClose()) return false;
		if (pxengine::PxeEngine::getInstance().getInputManager().getKeyboardFocusedWindow() == _window && _quitAction->getValue()) return false;
		return true;
	}

private:
	pxengine::PxeWindow* _window;
	pxengine::PxeAction* _quitAction;
};
#endif // !PXENGINESAMPELS_SCENES_CUBEWALL_H_
