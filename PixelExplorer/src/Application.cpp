#include "Application.h"

#include <new>

#include "PxeEngine.h"
#include "Log.h"
#include "scene/MainMenu.h"
#include "SDL_timer.h"

using namespace pxengine;
namespace pixelexplorer {
	Application::Application() {
		_state = INITIALIZING;
		_window = nullptr;
		_errorMenu = nullptr;
		_activeScene = nullptr;
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

	void Application::ChangeScene(UpdatableScene* scene)
	{
		getInstance().setActiveScene(scene);
	}

	void Application::onStart()
	{
		_state = RUNNING;
		PxeEngine& engine = pxeGetEngine();
		engine.getRenderPipeline().setVSyncMode(0);
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
	}

	void Application::onStop()
	{
		setActiveScene(nullptr);
		_errorMenu->drop();
		_errorMenu = nullptr;
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

		if (_activeScene) {
			_activeScene->update();
		}
	}

	void Application::setError()
	{
		setActiveScene(_errorMenu);
		_state = ERROR;
	}

	void Application::setError(const char* msg)
	{
		PEX_ERROR(msg);
		if (_state == ERROR) return;
		_errorMenu->setMessage(msg);
		setError();
	}

	void Application::setError(const std::string& msg)
	{
		PEX_ERROR(msg.c_str());
		if (_state == ERROR) return;
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
		if (_state == ERROR) return;
		if (_activeScene == scene) return;
		if (_activeScene) {
			_activeScene->stop();
			_window->setScene(nullptr);
			_activeScene->drop();
		}

		_activeScene = scene;
		if (_activeScene) {
			_activeScene->grab();
			_window->setScene(_activeScene->getScene());
			_activeScene->start(*_window);
		}
	}
}