#ifndef PXENGINE_WINDOW_H_
#define PXENGINE_WINDOW_H_
#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "PxeScene.h"

namespace pxengine {
	// Wrapper for SDL_Window
	class PxeWindow : public PxeGLAsset
	{
	public:
		// Sets the windows swap interval
		// Note: this will take effect at the start of next frame
		virtual void setSwapInterval(int8_t interval) = 0;

		// Returns the windows swap interval
		virtual PXE_NODISCARD int8_t getSwapInterval() = 0;

		// Returns if the user wants to close the window aka presses the X, Alt-F4...
		// Note: this can be reset/ignored by calling resetShouldClose()
		virtual PXE_NODISCARD bool getShouldClose() const = 0;

		// Resets the internal window close flag to false
		virtual void resetShouldClose() = 0;

		// Set if the window is hidden or not
		virtual void setWindowShown(bool show) = 0;

		// Returns if the window is "visible"
		// Note: does not check if a window is covering this one
		virtual PXE_NODISCARD bool getWindowShown() const = 0;

		// Sets the scene rendered
		virtual void setScene(PxeScene* scene) = 0;

		// Returns the current scene
		virtual PxeScene* getScene() const = 0;

		// Returns if this is the primary window
		virtual PXE_NODISCARD bool getPrimaryWindow() const = 0;

		// Returns the id used to create the window
		virtual PXE_NODISCARD PxeWindowId getWindowId() const = 0;

		// Returns the width of the window in pixels
		virtual PXE_NODISCARD int32_t getWindowWidth() const = 0;

		// Returns the height of the window in pixels
		virtual PXE_NODISCARD int32_t getWindowHeight() const = 0;

		// Sets {width} and {height} to the windows width and height
		// Note: width or height my be nullptr
		virtual void setWindowSize(int32_t width, int32_t height) = 0;

		// Returns the title of the window
		virtual PXE_NODISCARD const char* getWindowTitle() const = 0;

		// Sets the title of the window
		virtual void setWindowTitle(const char* title) = 0;

		//TODO Add window event callback functions

		virtual ~PxeWindow() = default;
		PxeWindow(const PxeWindow& other) = delete;
		PxeWindow operator=(const PxeWindow& other) = delete;

	protected:
		PxeWindow() = default;

	private:
		void PxeGLAsset::bind() {}
		void PxeGLAsset::unbind() {}
	};
}
#endif