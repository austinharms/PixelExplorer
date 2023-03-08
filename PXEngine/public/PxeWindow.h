#ifndef PXENGINE_WINDOW_H_
#define PXENGINE_WINDOW_H_
#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "PxeScene.h"
#include "PxeCamera.h"
#include "PxeRenderTarget.h"
PXE_PRIVATE_IMPLEMENTATION_START
#include "SDL_video.h"
PXE_PRIVATE_IMPLEMENTATION_END

namespace pxengine {
	class PxeEngine;

	// Wrapper for SDL_Window
	class PxeWindow : public PxeGLAsset, public PxeRenderTarget
	{
	public:
		// unused member for application use
		// Note: default value is nullptr
		void* userData;

		// Returns if the user wants to close the window aka presses the X, Alt-F4...
		// Note: this can be reset/ignored by calling resetShouldClose()
		PXE_NODISCARD bool getShouldClose() const;

		// Resets the internal window close flag to false
		void resetShouldClose();

		// Set if the window is hidden or not
		void setWindowShown(bool show);

		// Returns if the window is "visible"
		// Note: does not check if a window is covering this one
		PXE_NODISCARD bool getWindowShown() const;

		// Sets the scene rendered
		void setScene(PxeScene* scene);

		// Returns the current scene
		PxeScene* getScene() const;

		// Returns the width of the window in pixels
		PXE_NODISCARD int32_t getWindowWidth() const;

		// Returns the height of the window in pixels
		PXE_NODISCARD int32_t getWindowHeight() const;

		// Sets {width} and {height} to the windows width and height
		// Note: if width or height less then one this method will have no effect
		void setWindowSize(int32_t width, int32_t height);

		// Returns the title of the window
		PXE_NODISCARD const char* getWindowTitle() const;

		// Sets the title of the window
		void setWindowTitle(const char* title);

		// Set the PxeCamera used when the scene is rendered
		void setCamera(PxeCamera* camera);

		// Returns the PxeCamera used when the scene is rendered
		PXE_NODISCARD PxeCamera* getCamera() const;

		//TODO Add window event callback functions

		PXE_NODISCARD PxeCamera* getRenderCamera() const override;
		PXE_NODISCARD PxeRenderTexture* getRenderOutputTexture() const override;
		PXE_NODISCARD PxeScene* getRenderScene() const override;
		PXE_NODISCARD int32_t getRenderWidth() const override;
		PXE_NODISCARD int32_t getRenderHeight() const override;
		// update the SLD window and gui context and prepare them for rendering
		// Note: this binds the gui context
		// Note: this requires the primary OpenGl context
		void onPrepareForRender() override;
		void onRenderCompleted() override;
		virtual ~PxeWindow();
		PXE_NOCOPY(PxeWindow);

	protected:
		void initializeGl() override;
		void uninitializeGl() override;

	private:
		PxeWindow(int32_t width, int32_t height, const char* title);
		static constexpr PxeSize STORAGE_SIZE = 80;
		static constexpr PxeSize STORAGE_ALIGN = 8;
		std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
		PXE_PRIVATE_IMPLEMENTATION_START
	public:
		PXE_NODISCARD SDL_Window* getSDLWindow() const;
		PXE_NODISCARD uint32_t getSDLWindowId() const;
		PXE_NODISCARD PxeRenderTexture* getRenderTexture() const;
		void setShouldClose();
		void bindGuiContext();
		void prepareForRender();
		void bindGlContext();
		void setVsyncMode(int8_t mode);
		void acquireReadLock();
		void releaseReadLock();
		// Note: changes active OpenGl context
		void swapFramebuffers();

	private:
		friend class PxeEngine;
		struct Impl;
		enum WindowFlag : uint8_t
		{
			SIZE_CHANGED = 0b00000001,
			TITLE_CHANGED = 0b00000010,
			SWAP_CHANGED = 0b00000100,
			WINDOW_CLOSE = 0b00001000,
		};

		inline Impl& impl() { return reinterpret_cast<Impl&>(_storage); }
		inline const Impl& impl() const { return reinterpret_cast<const Impl&>(_storage); }
		PXE_NODISCARD bool getFlag(WindowFlag flag) const;
		void clearFlag(WindowFlag flag);
		void setFlag(WindowFlag flag, bool value);
		void setFlag(WindowFlag flag);
		// Note: deletes framebuffer for current OpenGl context
		void deleteFramebuffer();
		// Note: creates framebuffer for current OpenGl context
		// Note: rebinds glFramebuffer
		bool createFramebuffer();
		PXE_NODISCARD SDL_GLContext getGlContext() const;
		void setGlContext(SDL_GLContext ctx);
		void setSDLWindow(SDL_Window* window);
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif