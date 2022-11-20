#include "NpWindow.h"

#include "NpEngineBase.h"
#include "NpScene.h"
#include "SDL.h"
#include "NpLogger.h"

namespace pxengine::nonpublic {
	NpWindow::NpWindow(SDL_Window& window) : _sdlWindow(window)
	{
		_swapInterval = 1;
		_acquiredContext = false;
		_shouldClose = false;
		_hidden = SDL_GetWindowFlags(&_sdlWindow) | SDL_WINDOW_HIDDEN;
		_engineBase = &NpEngineBase::getInstance();
		_engineBase->grab();
	}

	NpWindow::~NpWindow()
	{
		_engineBase->removeWindowFromEventQueue(*this);
		SDL_DestroyWindow(&_sdlWindow);
		_engineBase->drop();
	}

	void NpWindow::acquireGlContext()
	{
		if (_acquiredContext) {
			PXE_WARN("Window Attempted to acquire Gl context multiple times ");
			return;
		}

		_engineBase->acquireGlContext(*this);
		_acquiredContext = true;
	}

	void NpWindow::releaseGlContext()
	{
		_acquiredContext = false;
		_engineBase->releaseGlContext(*this);
	}

	bool NpWindow::acquiredGlContext() const
	{
		return _acquiredContext;
	}

	void NpWindow::swapFrameBuffer()
	{
		SDL_GL_SwapWindow(&_sdlWindow);
	}

	const SDL_Window& NpWindow::getSDLWindow() const
	{
		return _sdlWindow;
	}

	SDL_Window& NpWindow::getSDLWindow()
	{
		return _sdlWindow;
	}

	void NpWindow::setSwapInterval(int8_t interval)
	{
		_swapInterval = interval;
	}

	int8_t NpWindow::getSwapInterval()
	{
		return _swapInterval;
	}

	bool NpWindow::getShouldClose() const
	{
		return _shouldClose;
	}

	void NpWindow::resetShouldClose()
	{
		_shouldClose = false;
	}

	void NpWindow::setWindowHidden(bool hidden)
	{
		if (_hidden == hidden) return;
		_hidden = hidden;
		if (hidden) {
			SDL_HideWindow(&_sdlWindow);
		}
		else {
			SDL_ShowWindow(&_sdlWindow);
		}
	}

	bool NpWindow::getWindowHidden() const
	{
		return _hidden;
	}

	void NpWindow::pollEvents()
	{
		_engineBase->pollEvents();
		SDL_Event e;
		while (_eventBuffer.size() > 0)
		{
			e = _eventBuffer.peek<SDL_Event>();
			_eventBuffer.pop<SDL_Event>();
			if (e.type == SDL_QUIT || e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
				_shouldClose = true;
			}

			switch (e.type)
			{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				char buf[512] = "";
				sprintf_s(buf, "Key Event Key: %c Pressed: %i, Repeat: %i", (char)e.key.keysym.sym, e.key.state, e.key.repeat);
				PXE_INFO(buf);
			}
			break;

			case SDL_MOUSEMOTION:
			{
				char buf[512] = "";
				sprintf_s(buf, "Mouse Move: %i,%i", e.motion.x, e.motion.y);
				PXE_INFO(buf);
			}
			break;
			}

			// TODO manage other events
		}
	}

	void NpWindow::setScene(PxeScene* scene)
	{
		if (scene) {
			_scene = dynamic_cast<NpScene*>(scene);
			if (!scene) {
				PXE_WARN("Scene pass did not inherit from NpScene, Scene set to nullptr");
			}
		}
		else {
			_scene = nullptr;
		}
	}

	void NpWindow::drawScene(PxeScene* scene)
	{
		NpScene* activeScene;
		if (scene) {
			activeScene = dynamic_cast<NpScene*>(scene);
		}
		else {
			activeScene = _scene;
		}

		if (!activeScene) {
			PXE_WARN("Attempted to draw null scene");
			return;
		}

		activeScene->grab();
		bool shouldReleaseContext = !_acquiredContext;
		if (shouldReleaseContext) acquireGlContext();
		//TODO implement this 
		if (shouldReleaseContext) releaseGlContext();
		activeScene->drop();
	}

	PxeScene* NpWindow::getScene() const
	{
		return _scene;
	}

	const PxeRingBuffer<SDL_Event, NpWindow::WINDOW_EVENT_QUEUE_SIZE>& NpWindow::getEventBuffer() const
	{
		return _eventBuffer;
	}

	PxeRingBuffer<SDL_Event, NpWindow::WINDOW_EVENT_QUEUE_SIZE>& NpWindow::getEventBuffer()
	{
		return _eventBuffer;
	}

	void NpWindow::onDelete()
	{
	}
}
