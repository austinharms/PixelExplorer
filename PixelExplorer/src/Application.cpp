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
		if (_window) {
			_window->drop();
			_window = nullptr;
		}

		setActiveScene(nullptr);
		_errorMenu->drop();
		_errorMenu = nullptr;
	}

	void Application::onUpdate()
	{
		if (!_window) return;
		if (_window->getShouldClose()) {
			_window->drop();
			_window = nullptr;
			return;
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

	void Application::setActiveScene(UpdatableScene* scene)
	{
		if (_activeScene)
			_activeScene->drop();
		_activeScene = scene;
		if (_activeScene) {
			_activeScene->grab();
			if (_window)
				_window->setScene(_activeScene->getScene());
		}
		else if(_window) {
			_window->setScene(nullptr);
		}
	}
}