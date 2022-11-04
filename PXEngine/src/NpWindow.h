#include <stdint.h>

#include "PxeWindow.h"
#include "SDL.h"

#ifndef PXENGINE_NONPUBLIC_WINDOW_H_
#define PXENGINE_NONPUBLIC_WINDOW_H_
namespace pxengine::nonpublic {
	class NpWindow : public PxeWindow
	{
	public:
		NpWindow(SDL_Window& window);
		virtual ~NpWindow();

		// blocks the calling thread until the OpenGl context is ready for use with this window on the calling thread
		// note you MUST call releaseGlContext after you are done making OpenGl calls
		void acquireGlContext() override;

		// releases the OpenGl context for use on other threads and windows
		void releaseGlContext() override;

		// returns true if this window has currently acquired the OpenGl context
		bool acquiredGlContext() const override;

		// swaps the windows front and back frame buffer
		void swapFrameBuffer() override;

		// returns the internal SDL Window object
		const SDL_Window& getSDLWindow() const;

		// returns the internal SDL Window object
		SDL_Window& getSDLWindow();

		void setSwapInterval(int8_t interval) override;

		int8_t getSwapInterval() override;

		bool getShouldClose() const override;

		void resetShouldClose() override;

		// TODO add some sort of event system
		void pollEvents() override;

	protected:
		void onDelete() override;

	private:
		SDL_Window& _sdlWindow;
		int8_t _swapInterval;
		bool _acquiredContext;
		bool _shouldClose;
	};
}
#endif // !PXENGINE_NONPUBLIC_WINDOW_H_
