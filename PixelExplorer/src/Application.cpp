#include "Application.h"

#include <new>

#include "PxeEngine.h"
#include "Log.h"
#include "scenes/title/TitleScene.h"
#include "SDL_timer.h"

using namespace pxengine;
namespace pixelexplorer {
	Application::Application() {
		_state = INITIALIZING;
		_window = nullptr;
		_currentScene = nullptr;
	}

	Application& Application::getInstance()
	{
		static Application app;
		return app;
	}

	void Application::switchScene(ApplicationScene* scene)
	{
		if (_currentScene == scene) return;
		if (_currentScene) {
			_currentScene->onStop();
			_window->setScene(nullptr);
			_currentScene->drop();
		} 

		if ((_currentScene = scene)) {
			scene->grab();
			_window->setScene(&(scene->getScene()));
			scene->onStart(*_window);
		}
	
	}

	void Application::onStart()
	{
		_state = RUNNING;
		PxeEngine& engine = PxeEngine::getInstance();
		engine.getRenderPipeline().setVSyncMode(0);
		_window = engine.createWindow(600, 400, "Pixel Explorer");
		if (!_window) { PEX_FATAL("Failed to create main window"); }

		scenes::title::TitleScene* title = new(std::nothrow) scenes::title::TitleScene();
		if (!title) { PEX_FATAL("Failed to allocate TitleScene"); }
		switchScene(title);
		title->drop();
	}

	void Application::onStop()
	{
		switchScene(nullptr);
		_window->drop();
		_window = nullptr;
		_state = STOPPED;
	}

	void Application::onUpdate()
	{
		if (_window->getShouldClose()) {
			_window->resetShouldClose();
			quit();
		}

		if (_currentScene) {
			_currentScene->onUpdate();
		}
	}

	void Application::quit()
	{
		bool shouldStop = true;
		if (_currentScene) {
			_currentScene->onClose(shouldStop);
		}

		if (shouldStop) {
			switchScene(nullptr);
			PxeEngine::getInstance().shutdown();
		}
	}
}