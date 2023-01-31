#ifndef PXENGINE_NONPUBLIC_WINDOW_H_
#define PXENGINE_NONPUBLIC_WINDOW_H_
#include <mutex>
#include <shared_mutex>

#include "PxeTypes.h"
#include "PxeWindow.h"
#include "SDL_video.h"
#include "PxeScene.h"
#include "NpScene.h"
#include "imgui.h"
#include "PxeCamera.h"
#include "PxeRenderTexture.h"

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
			PXE_NODISCARD bool getPrimaryWindow() const override;
			PXE_NODISCARD int32_t getWindowWidth() const override;
			PXE_NODISCARD int32_t getWindowHeight() const override;
			void setWindowSize(int32_t width, int32_t height) override;
			PXE_NODISCARD const char* getWindowTitle() const override;
			void setWindowTitle(const char* title) override;
			void setCamera(PxeCamera* camera) override;
			PXE_NODISCARD PxeCamera* getCamera() const override;
			PXE_NODISCARD void* getUserData() const override;
			void setUserData(void* data) override;


			//############# PRIVATE API ##################

			NpWindow(int32_t width, int32_t height, const char* title);
			virtual ~NpWindow();
			PXE_NODISCARD SDL_Window* getSDLWindow() const;
			PXE_NODISCARD uint32_t getSDLWindowId() const;
			PXE_NODISCARD NpScene* getNpScene() const;
			void setShouldClose();
			void setPrimaryWindow();
			void bindGuiContext();
			void updateSDLWindow();
			// Note: this method assumes valid OpenGl context
			void setVsyncMode(int8_t mode);
			void acquireReadLock();
			void releaseReadLock();
			void acquireWriteLock();
			void releaseWriteLock();

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
				PRIMARY_WINDOW		= 0b00000100,
				WINDOW_CLOSE		= 0b00001000,
			};

			bool getFlag(WindowFlag flag) const;
			void clearFlag(WindowFlag flag);
			void setFlag(WindowFlag flag, bool value);
			void setFlag(WindowFlag flag);

			mutable std::shared_mutex _windowMutex;
			void* _userData;
			char* _title;
			NpScene* _scene;
			PxeCamera* _camera;
			SDL_Window* _sdlWindow;
			SDL_GLContext _sdlGlContext;
			ImGuiContext* _guiContext;
			PxeRenderTexture* _renderTexture;
			uint32_t _externalFramebuffer;
			int32_t _width;
			int32_t _height;
			uint8_t _flags;
			int8_t _vsyncMode;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_WINDOW_H_
