#ifndef PXENGINE_NONPUBLIC_WINDOW_H_
#define PXENGINE_NONPUBLIC_WINDOW_H_
#include <shared_mutex>

#include "PxeWindow.h"
#include "PxeRenderTexture.h"
#include "NpScene.h"
#include "SDL_video.h"
#include "imgui.h"

namespace pxengine {
	namespace nonpublic {
		class NpEngine;

		class NpWindow : public PxeWindow
		{
		public:
			//############# PxeWindow API ##################

			PXE_NODISCARD bool getShouldClose() const override;
			void resetShouldClose() override;
			void setWindowShown(bool show) override;
			PXE_NODISCARD bool getWindowShown() const override;
			void setScene(PxeScene* scene) override;
			PXE_NODISCARD PxeScene* getScene() const override;
			PXE_NODISCARD int32_t getWindowWidth() const override;
			PXE_NODISCARD int32_t getWindowHeight() const override;
			void setWindowSize(int32_t width, int32_t height) override;
			PXE_NODISCARD const char* getWindowTitle() const override;
			void setWindowTitle(const char* title) override;
			void setCamera(PxeCameraInterface* camera) override;
			PXE_NODISCARD PxeCameraInterface* getCamera() const override;
			PXE_NODISCARD void* getUserData() const override;
			void setUserData(void* data) override;


			//############# PRIVATE API ##################

			NpWindow(int32_t width, int32_t height, const char* title);
			virtual ~NpWindow();
			PXE_NODISCARD SDL_Window* getSDLWindow() const;
			PXE_NODISCARD uint32_t getSDLWindowId() const;
			PXE_NODISCARD NpScene* getNpScene() const;
			PXE_NODISCARD PxeRenderTexture* getRenderTexture() const;
			void setShouldClose();
			void bindGuiContext();
			// update the SLD window and gui context and prepare them for rendering
			// Note: this binds the gui context
			// Note: this requires the primary OpenGl context
			void prepareForRender();
			void bindGlContext();
			void setVsyncMode(int8_t mode);
			void acquireReadLock();
			void releaseReadLock();
			// Note: changes active OpenGl context
			void swapFramebuffers();

		protected:

			//############# PxeGlAsset API ##################

			void initializeGl() override;
			void uninitializeGl() override;

		private:
			friend class NpEngine;
			enum WindowFlag : uint8_t
			{
				SIZE_CHANGED		= 0b00000001,
				TITLE_CHANGED		= 0b00000010,
				SWAP_CHANGED		= 0b00000100,
				WINDOW_CLOSE		= 0b00001000,
			};

			bool getFlag(WindowFlag flag) const;
			void clearFlag(WindowFlag flag);
			void setFlag(WindowFlag flag, bool value);
			void setFlag(WindowFlag flag);
			// Note: deletes framebuffer for current OpenGl context
			void deleteFramebuffer();
			// Note: creates framebuffer for current OpenGl context
			// Note: rebinds glFramebuffer
			bool createFramebuffer();

			mutable std::shared_mutex _windowMutex;
			void* _userData;
			char* _title;
			NpScene* _scene;
			PxeCameraInterface* _camera;
			SDL_Window* _sdlWindow;
			SDL_GLContext _sdlGlContext;
			ImGuiContext* _guiContext;
			PxeRenderTexture* _renderTexture;
			uint32_t _internalFramebuffer;
			int32_t _width;
			int32_t _height;
			uint8_t _flags;
			int8_t _vsyncMode;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_WINDOW_H_
