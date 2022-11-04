#include "NpWindow.h"

#include "NpEngineBase.h"
#include "SDL.h"

namespace pxengine::nonpublic {
	NpWindow::NpWindow(SDL_Window& window) : _sdlWindow(window)
	{
		_swapInterval = 1;
		_acquiredContext = false;
		_shouldClose = false;
	}

	NpWindow::~NpWindow()
	{
		SDL_DestroyWindow(&_sdlWindow);
	}

	void NpWindow::acquireGlContext()
	{
		NpEngineBase::getInstance().acquireGlContext(*this);
		_acquiredContext = true;
	}

	void NpWindow::releaseGlContext()
	{
		_acquiredContext = false;
		NpEngineBase::getInstance().releaseGlContext(*this);
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

	void NpWindow::pollEvents()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT) {
				_shouldClose = true;
			}
			else
			{
				// TODO manage other events
			}
		}
	}

	void NpWindow::onDelete()
	{
	}
}
