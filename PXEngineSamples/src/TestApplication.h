#ifndef PXENGINESAMPELS_TEST_APPLICATION_H_
#define PXENGINESAMPELS_TEST_APPLICATION_H_
#include <new>
#include <string>
#include <list>
#include <cstdlib>
#include <ctime>

#include "PxeEngineAPI.h"
#include "imgui.h"
#include "TestRenderObject.h"
#include "PhysicsRenderable.h"
#include "TestGuiElement.h"
#include "SDL_keycode.h"

class TestApplication : public pxengine::PxeApplicationInterface
{
public:
	TestApplication() {
		_mainWindow = nullptr;
		_testScene = nullptr;
		_testGui = nullptr;
		_createAction = nullptr;
		_windowCounter = 0;
	}

	void onStart() override {
		using namespace pxengine;
		PxeEngine& engine = pxeGetEngine();
		pxengine::PxeInputManager& inputMgr = pxengine::pxeGetEngine().getInputManager();
		PxeActionSource* createSource = inputMgr.getActionSource((PxeActionSourceCode)PxeActionSourceType::KEYBOARD << 32 | SDLK_c);
		_createAction = inputMgr.getAction("CreateWindow");
		_createAction->grab();
		_createAction->addSource(*createSource);

		_mainWindow = engine.createWindow(600, 400, "Main Window");

		PxeScene* mainScene = engine.createScene();
		_testGui = new(std::nothrow) TestGuiElement();
		mainScene->addRenderable(*_testGui);
		_mainWindow->setScene(mainScene);

		PxeShader* shader = engine.loadShader(getAssetPath("shaders") / "test.pxeshader");
		PxeTexture* texture = new(std::nothrow) PxeTexture();
		texture->loadImage(pxengine::getAssetPath("textures") / "test.png");
		PxeRenderMaterial* material = new(std::nothrow) PxeRenderMaterial(*shader);
		material->setTexture("u_Texture", *texture, 0);
		material->setProperty4f("u_Color", glm::vec4(1, 1, 1, 1));

		_testScene = engine.createScene();
		//_testScene->setPhysicsSimulationSpeed(1000);
		PhysicsRenderable* basePhysObj = PhysicsRenderable::createPhysicsRenderable(glm::vec3(0, -10, 0), false, *material);
		_testScene->addRenderable(*basePhysObj);
		for (int32_t y = -9; y < 1000; ++y) {
			glm::vec3 force(0);
			if (y == 0) {
				force.x = -1;
			}

			PhysicsRenderable* testObj = PhysicsRenderable::createPhysicsRenderable(glm::vec3(0, y * 1.1f, 0), true, *material, basePhysObj->getIndexBuffer(), basePhysObj->getVertexArray(), basePhysObj->getShape(), force);
			_testScene->addRenderable(*testObj);
			testObj->drop();
		}

		//srand(static_cast<uint32_t>(time(nullptr)));
		//TestRenderObject* baseObj = new(std::nothrow) TestRenderObject(*material);
		//for (int32_t x = -25; x < 26; ++x) {
		//	for (int32_t y = -25; y < 26; ++y) {
		//		TestRenderObject* testObj = new(std::nothrow) TestRenderObject(*material, baseObj->getIndexBuffer(), baseObj->getVertexArray());
		//		testObj->translate(glm::vec3(x, y, 0));
		//		testObj->rotateAbout(glm::vec3(1, 0, 0), ((float)rand() / (float)RAND_MAX) * glm::two_pi<float>());
		//		testObj->rotateAbout(glm::vec3(0, 1, 0), ((float)rand() / (float)RAND_MAX) * glm::two_pi<float>());
		//		testObj->rotateAbout(glm::vec3(0, 0, 1), ((float)rand() / (float)RAND_MAX) * glm::two_pi<float>());
		//		_testScene->addRenderable(*testObj);
		//		testObj->drop();
		//	}
		//}

		//baseObj->drop();


		texture->drop();
		shader->drop();
		material->drop();
		mainScene->drop();
	}

	void onUpdate() override {
		if (_createAction && _createAction->getValue()) {
			pxengine::PxeWindow* window = pxengine::pxeGetEngine().createWindow(600, 400, ("Window " + std::to_string(_windowCounter++)).c_str());
			window->setScene(_testScene);
			_windows.push_back(window);
		}

		if (_mainWindow && _mainWindow->getShouldClose()) {
			_mainWindow->drop();
			_mainWindow = nullptr;
			return;
		}

		if (_testGui->getClicked()) {
			pxengine::PxeWindow* window = pxengine::pxeGetEngine().createWindow(600, 400, ("Window " + std::to_string(_windowCounter++)).c_str());
			window->setScene(_testScene);
			_windows.push_back(window);
			_testGui->resetClicked();
		}

		auto it = _windows.begin();
		while (it != _windows.end())
		{
			if ((*it)->getShouldClose()) {
				(*it)->drop();
				it = _windows.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void onStop() override {
		if (_createAction) {
			_createAction->drop();
			_createAction = nullptr;
		}

		_testScene->drop();
		_testScene = nullptr;
		_testGui->drop();
		_testGui = nullptr;

		for (pxengine::PxeWindow* window : _windows)
			window->drop();
		_windows.clear();

		if (_mainWindow) {
			_mainWindow->drop();
			_mainWindow = nullptr;
		}
	}

private:
	pxengine::PxeWindow* _mainWindow;
	pxengine::PxeScene* _testScene;
	pxengine::PxeAction* _createAction;
	uint32_t _windowCounter;
	TestGuiElement* _testGui;
	std::list<pxengine::PxeWindow*> _windows;
};
#endif // !PXENGINESAMPELS_TEST_APPLICATION_H_
