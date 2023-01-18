#ifndef PXENGINESAMPELS_TEST_APPLICATION_H_
#define PXENGINESAMPELS_TEST_APPLICATION_H_
#include <new>
#include <string>

#include "PxeEngineAPI.h"
#include "imgui.h"
#include "TestTextureRenderObject.h"

class TestApplication : public pxengine::PxeApplicationInterface
{
public:
	TestApplication() {
		_mainWindow = nullptr;
		_testWindow = nullptr;
		_testScene = nullptr;
		_windowCounter = 0;
	}

	void onStart() override {
		using namespace pxengine;
		PxeEngine& engine = pxeGetEngine();
		_mainWindow = engine.createWindow(600, 400, "Main Window");
		_testWindow = engine.createWindow(600, 400, ("Window " + std::to_string(_windowCounter++)).c_str());
		_testScene = engine.createScene();
		_mainWindow->setScene(_testScene);
		_testWindow->setScene(_testScene);

		PxeShader* shader = engine.loadShader(getAssetPath("shaders") / "test.pxeshader");
		PxeRenderMaterial* material = new(std::nothrow) PxeRenderMaterial(*shader);
		shader->drop();
		shader = nullptr;

		PxeTexture* texture = new(std::nothrow) PxeTexture();
		texture->loadImage(pxengine::getAssetPath("textures") / "test.png");
		material->setTexture("u_Texture", *texture, 0);
		material->setProperty4f("u_Color", glm::vec4(1, 1, 1, 1));
		texture->drop();
		texture = nullptr;

		TestTexturedRenderObject* testObj = new(std::nothrow) TestTexturedRenderObject(*material);
		material->drop();
		material = nullptr;

		_testScene->addRenderable(*testObj);
		testObj->drop();
		testObj = nullptr;
	}

	void onUpdate() override {
		if (_mainWindow && _mainWindow->getShouldClose()) {
			_mainWindow->drop();
			_mainWindow = nullptr;
		}

		if (_testWindow && _testWindow->getShouldClose()) {
			_testWindow->drop();
			_testWindow = pxengine::pxeGetEngine().createWindow(600, 400, ("Window " + std::to_string(_windowCounter++)).c_str());
			_testWindow->setScene(_testScene);
		}
	}

	void onStop() override {
		_testScene->drop();
		_testScene = nullptr;

		if (_mainWindow) {
			_mainWindow->drop();
			_mainWindow = nullptr;
		}

		if (_testWindow) {
			_testWindow->drop();
			_testWindow = nullptr;
		}
	}

	void preGUI(pxengine::PxeWindow& window) override {
		ImGui::ShowDemoWindow();
	}

private:
	pxengine::PxeWindow* _mainWindow;
	pxengine::PxeWindow* _testWindow;
	pxengine::PxeScene* _testScene;
	uint32_t _windowCounter;
};
#endif // !PXENGINESAMPELS_TEST_APPLICATION_H_
