#include "NpWindow.h"

#include "NpEngineBase.h"

namespace pxengine::nonpublic {
	NpWindow::NpWindow(SDL_Window& window) : _sdlWindow(window)
	{
		_acquiredContext = false;
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

	void NpWindow::onDelete()
	{
	}
}
