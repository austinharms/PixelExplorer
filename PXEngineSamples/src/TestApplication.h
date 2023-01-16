#ifndef PXENGINESAMPELS_TEST_APPLICATION_H_
#define PXENGINESAMPELS_TEST_APPLICATION_H_
#include "PxeEngineAPI.h"
#include "imgui.h"

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

	void preGUI(pxengine:: PxeWindow& window) override {
		ImGui::ShowDemoWindow();
	}


private:
	pxengine::PxeWindow* _mainWindow;
};
#endif // !PXENGINESAMPELS_TEST_APPLICATION_H_
