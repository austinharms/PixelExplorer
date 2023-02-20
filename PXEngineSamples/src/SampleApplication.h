#ifndef PXENGINESAMPELS_SAMPLE_APPLICATION_H_
#define PXENGINESAMPELS_SAMPLE_APPLICATION_H_
#include <new>
#include <list>

#include "PxeEngineAPI.h"
#include "MainMenu.h"
#include "SampleSceneBase.h"
#include "scenes/CubeWall/CubeWall.h"
#include "scenes/CubeStack/CubeStack.h"
#include "scenes/MarchingCubes/MarchingCubes.h"
#include "scenes/FreeCamera/FreeCamera.h"

class SampleApplication : public pxengine::PxeApplicationInterface
{
public:
	SampleApplication() {
		_mainWindow = nullptr;
		_mainMenu = nullptr;
		_error = false;
	}

	void onStart() override {
		using namespace pxengine;
		PxeEngine& engine = pxeGetEngine();
		engine.getRenderPipeline().setVSyncMode(0);
		_mainWindow = engine.createWindow(600, 400, "PXEngine Sample Application");
		if (!_mainWindow) {
			_error = true;
			return;
		}

		PxeScene* scene = engine.createScene();
		if (!scene) {
			_error = true;
			return;
		}

		_mainWindow->setScene(scene);
		_mainMenu = new(std::nothrow) MainMenu();
		if (!_mainMenu) {
			_error = true;
			scene->drop();
			return;
		}

		scene->addObject(*_mainMenu);
		scene->drop();
	}

	void onUpdate() override {
		if (_error || _mainWindow->getShouldClose()) {
			pxengine::pxeGetEngine().shutdown();
			return;
		}

		uint8_t menuAction = _mainMenu->getClickedActions();
		if (menuAction & MainMenu::QUIT)
		{
			pxengine::pxeGetEngine().shutdown();
			return;
		}

		if (menuAction & MainMenu::CUBEWALL)
		{
			CubeWall* scene = new(std::nothrow) CubeWall();
			if (scene) _runningSamples.push_back(scene);
		}

		if (menuAction & MainMenu::CUBESTACK)
		{
			CubeStack* scene = new(std::nothrow) CubeStack();
			if (scene) _runningSamples.push_back(scene);
		}

		if (menuAction & MainMenu::MARCHINGCUBES)
		{
			MarchingCubes* scene = new(std::nothrow) MarchingCubes();
			if (scene) _runningSamples.push_back(scene);
		}

		if (menuAction & MainMenu::FREECAMERA)
		{
			FreeCamera* scene = new(std::nothrow) FreeCamera();
			if (scene) _runningSamples.push_back(scene);
		}

		auto it = _runningSamples.begin();
		while (it != _runningSamples.end())
		{
			if (!(*it)->update()) {
				delete (*it);
				it = _runningSamples.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void onStop() override {
		for (SampleSceneBase* sample : _runningSamples)
			delete sample;
		_runningSamples.clear();

		if (_mainWindow) {
			_mainWindow->drop();
			_mainWindow = nullptr;
		}

		if (_mainMenu) {
			_mainMenu->drop();
			_mainMenu = nullptr;
		}
	}

private:
	pxengine::PxeWindow* _mainWindow;
	MainMenu* _mainMenu;
	std::list<SampleSceneBase*> _runningSamples;
	bool _error;
};
#endif // !PXENGINESAMPELS_SAMPLE_APPLICATION_H_
