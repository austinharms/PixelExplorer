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
		_frameCount = 0;
		_frameCountTimer = 1000;
		_lastFrameCount = 0;
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
		engine.setVSyncMode(0);
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
		if (SDL_GetTicks64() > +_frameCountTimer) {
			_frameCountTimer = SDL_GetTicks64() + 250;
			_lastFrameCount = _frameCount * 4;
			_frameCount = 0;
		}

		++_frameCount;
		if (_window->getShouldClose()) {
			_window->resetShouldClose();
			quit();
		}

		if (_activeScene) {
			_activeScene->update();
		}
	}

	void Application::postGUI(pxengine::PxeWindow& window)
	{
		char fpsText[16];
		sprintf_s(fpsText, "FPS: %u", _lastFrameCount);
		ImGui::PushFont(nullptr);
		ImVec2 textSize = ImGui::CalcTextSize(fpsText);
		constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_UnsavedDocument;
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(textSize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("FPS TEXT", nullptr, flags);
		ImGui::Text(fpsText);
		ImGui::End();
		ImGui::PopStyleVar(2);
		ImGui::PopFont();
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