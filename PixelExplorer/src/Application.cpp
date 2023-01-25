#include "Application.h"

#include <new>

#include "PxeEngine.h"
#include "Log.h"
#include "scene/MainMenu.h"

using namespace pxengine;
namespace pixelexplorer {
	Application::Application() {
		_window = nullptr;
		_errorMenu = nullptr;
		_activeScene = nullptr;
		_state = NONE;
	}

	Application& Application::getInstance()
	{
		static Application app;
		return app;
	}

	void Application::Error()
	{
		getInstance().setError();
	}

	void Application::Error(const char* msg)
	{
		getInstance().setError(msg);
	}

	void Application::Error(const std::string& msg)
	{
		getInstance().setError(msg);
	}

	void Application::onStart()
	{
		PxeEngine& engine = pxeGetEngine();
		_window = engine.createWindow(600, 400, "Pixel Explorer");
		if (!_window) {
			PEX_FATAL("Failed to create main window");
		}

		_errorMenu = new(std::nothrow) scene::ErrorMenu();
		if (!_errorMenu) {
			PEX_FATAL("Failed to allocate error menu");
		}

		scene::MainMenu* menu = new(std::nothrow) scene::MainMenu();
		if (!menu) {
			setError("Failed to allocate MainMenu scene");
			return;
		}

		setActiveScene(menu);
		menu->drop();
		_state = MAIN_MENU;
	}

	void Application::onStop()
	{
		setActiveScene(nullptr);
		_errorMenu->drop();
		_errorMenu = nullptr;
		_window->drop();
		_window = nullptr;
	}

	void Application::onUpdate()
	{
		if (_window->getShouldClose()) {
			_window->resetShouldClose();
			quit();
		}

		if (_activeScene) {
			_activeScene->update();
		}
	}

	void Application::setError()
	{
		_state = ERROR;
		setActiveScene(_errorMenu);
	}

	void Application::setError(const char* msg)
	{
		_errorMenu->setMessage(msg);
		setError();
	}

	void Application::setError(const std::string& msg)
	{
		_errorMenu->setMessage(msg);
		setError();
	}

	void Application::quit()
	{
		bool shouldQuit = true;
		if (_activeScene) {
			_activeScene->quit(shouldQuit);
		}

		if (shouldQuit) {
			setActiveScene(nullptr);
			pxeGetEngine().shutdown();
		}
	}

	void Application::setActiveScene(UpdatableScene* scene)
	{
		if (_activeScene) {
			_window->setScene(nullptr);
			_activeScene->drop();
		}
		_activeScene = scene;
		if (_activeScene) {
			_activeScene->grab();
			_window->setScene(_activeScene->getScene());
		}
	}
}