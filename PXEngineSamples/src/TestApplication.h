#ifndef PXENGINESAMPELS_TEST_APPLICATION_H_
#define PXENGINESAMPELS_TEST_APPLICATION_H_
#include <new>

#include "PxeEngineAPI.h"
#include "imgui.h"
#include "TestTextureRenderObject.h"

class TestApplication : public pxengine::PxeApplicationInterface
{
public:
	TestApplication() {
		_mainWindow = nullptr;
	}

	void onStart() override {
		using namespace pxengine;
		PxeEngine& engine = pxeGetEngine();
		_mainWindow = engine.createWindow(600, 400, "Main Window", 0);
		PxeScene* scene = engine.createScene();
		_mainWindow->setScene(scene);

		PxeShader* shader = engine.loadShader(getAssetPath("shaders") / "test.pxeshader");
		PxeRenderMaterial* material = new(std::nothrow) PxeRenderMaterial(*shader);
		shader->drop();
		shader = nullptr;

		PxeRenderTexture* texture = new(std::nothrow) PxeRenderTexture();
		texture->loadImage(pxengine::getAssetPath("textures") / "test.png");
		material->setTexture("u_Texture", *texture, 0);
		material->setProperty4f("u_Color", glm::vec4(1, 1, 1, 1));
		texture->drop();
		texture = nullptr;

		TestTexturedRenderObject* testObj = new(std::nothrow) TestTexturedRenderObject(*material);
		material->drop();
		material = nullptr;

		scene->addRenderable(*testObj);
		testObj->drop();
		testObj = nullptr;

		scene->drop();
		scene = nullptr;
	}

	void onUpdate() override {
		if (_mainWindow && _mainWindow->getShouldClose()) {
			_mainWindow->drop();
			_mainWindow = nullptr;
		}
	}

	void onStop() override {
		if (_mainWindow) {
			_mainWindow->drop();
			_mainWindow = nullptr;
		}
	}

	void preGUI(pxengine::PxeWindow& window) override {
		ImGui::ShowDemoWindow();
	}


private:
	pxengine::PxeWindow* _mainWindow;
};
#endif // !PXENGINESAMPELS_TEST_APPLICATION_H_
