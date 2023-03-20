#ifndef PXENGINESAMPELS_SAMPLE_APPLICATION_H_
#define PXENGINESAMPELS_SAMPLE_APPLICATION_H_
#include <new>
#include <list>

#include "PxeEngineAPI.h"
#include "MainMenu.h"
#include "SampleSceneBase.h"
#include "samples/FreeCamera/FreeCameraSample.h"
#include "samples/CubeWall/CubeWallSample.h"
#include "samples/CubeStack/CubeStack.h"

class SampleApplication : public pxengine::PxeApplication
{
public:
	SampleApplication() {
		_mainWindow = nullptr;
		_mainMenu = nullptr;
		_error = false;
	}

	void onStart() override {
		using namespace pxengine;
		PxeEngine& engine = PxeEngine::getInstance();
		engine.getRenderPipeline().setVSyncMode(0);
		_mainWindow = engine.createWindow(600, 400, "PXEngine Sample Application");
		if (!_mainWindow) {
			_error = true;
			return;
		}

		PxeScene& scene = *engine.createScene();
		PxeObject& menuObject = MainMenu::createMainMenuObject();
		_mainMenu = menuObject.getExactComponent<MainMenu>();
		_mainMenu->grab();
		scene.addObject(menuObject);
		menuObject.drop();
		_mainWindow->setScene(&scene);
		scene.drop();
	}

	void onUpdate() override {
		if (_error || _mainWindow->getShouldClose()) {
			pxengine::PxeEngine::getInstance().shutdown();
			return;
		}

		uint8_t menuAction = _mainMenu->getClickedActions();
		if (menuAction & MainMenu::QUIT)
		{
			pxengine::PxeEngine::getInstance().shutdown();
			return;
		}

		if (menuAction & MainMenu::CUBEWALL)
		{
			CubeWallSample* scene = new CubeWallSample();
			_runningSamples.push_back(scene);
		}

		if (menuAction & MainMenu::CUBESTACK)
		{
			CubeStackSample* scene = new CubeStackSample();
			_runningSamples.push_back(scene);
		}

		if (menuAction & MainMenu::FREECAMERA)
		{
			FreeCameraSample* scene = new FreeCameraSample();
			_runningSamples.push_back(scene);
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

		if (_mainMenu) {
			_mainMenu->drop();
			_mainMenu = nullptr;
		}

		if (_mainWindow) {
			_mainWindow->drop();
			_mainWindow = nullptr;
		}
	}

private:
	pxengine::PxeWindow* _mainWindow;
	MainMenu* _mainMenu;
	std::list<SampleSceneBase*> _runningSamples;
	bool _error;
};
#endif // !PXENGINESAMPELS_SAMPLE_APPLICATION_H_
