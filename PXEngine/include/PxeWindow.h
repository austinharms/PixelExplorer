#include <stdint.h>

#include "PxeRefCount.h"

#ifndef PXENGINE_WINDOW_H_
#define PXENGINE_WINDOW_H_
namespace pxengine {
	class PxeWindow : public PxeRefCount 
	{
	public:
		virtual ~PxeWindow() {}

		// blocks the calling thread until the OpenGl context is ready for use with this window on the calling thread
		// note you MUST call releaseGlContext after you are done making OpenGl calls
		virtual void acquireGlContext() = 0;

		// releases the OpenGl context for use on other threads and windows
		virtual void releaseGlContext() = 0;

		// returns true if this window has currently acquired the OpenGl context
		virtual bool acquiredGlContext() const = 0;

		// swaps the windows front and back frame buffer
		virtual void swapFrameBuffer() = 0;

		// sets the windows swap interval
		// note this will only take effect after the next call to acquireGlContext
		virtual void setSwapInterval(int8_t interval) = 0;

		// returns the windows swap interval
		virtual int8_t getSwapInterval() = 0;

		// retruns true if the user wants to close the window aka presses the X, Alt-F4...
		// this can be reset by calling resetShouldClose
		virtual bool getShouldClose() const = 0;

		// resets the internal window close flag to false
		virtual void resetShouldClose() = 0;

		// processes the windows event queue, this should be done every frame to allow for user interaction
		// note not calling this will cause windows to think the application is not responding
		// TODO add some sort of event system
		virtual void pollEvents() = 0;
	};
}
#endif