#include <stdint.h>

#include "PxeRefCount.h"
#include "PxeScene.h"
#include "PxeGLAsset.h"
#include "SDL_events.h"

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
		// this can be reset/ignored by calling resetShouldClose
		virtual bool getShouldClose() const = 0;

		// resets the internal window close flag to false
		virtual void resetShouldClose() = 0;

		// updates the window event buffer
		// returns false if the event buffer is empty
		// if event is not null sets and removes the first event from the buffer and returns true
		// if event is null returns true if there is an event in the queue
		// note this MUST be called on the same thread that created the window
		virtual bool pollEvents(SDL_Event* event = nullptr) = 0;

		// set if the window is hidden or not
		// this is false by default
		virtual void setWindowShown(bool show) = 0;

		// returns true if the window is "visible"
		virtual bool getWindowShown() const = 0;

		// sets scene rendered when calling drawScene
		// to clear the current scene pass nullptr as the scene
		virtual void setScene(PxeScene* scene) = 0;

		// draws the provided scene to the frame buffer
		// if the provided scene is nullptr it will draw the scene set using setScene
		// if that scene is also nullptr the call will throw a warning and draw nothing
		virtual void drawScene(PxeScene* scene = nullptr) = 0;

		// returns a pointer to the current scene
		virtual PxeScene* getScene() const = 0;

		PxeWindow() = default;
		PxeWindow(const PxeWindow& other) = delete;
		PxeWindow operator=(const PxeWindow& other) = delete;
	};
}
#endif