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
		virtual void setSwapInterval(int8_t interval) = 0;

		// returns the windows swap interval
		virtual int8_t getSwapInterval() = 0;
	};
}
#endif