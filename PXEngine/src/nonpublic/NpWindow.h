#ifndef PXENGINE_NONPUBLIC_WINDOW_H_
#define PXENGINE_NONPUBLIC_WINDOW_H_
#include "PxeTypes.h"
#include "PxeWindow.h"
#include "SDL_video.h"
#include "PxeScene.h"
#include "NpScene.h"
#include "imgui.h"
#include "PxeCamera.h"

namespace pxengine {
	namespace nonpublic {
		class NpWindow : public PxeWindow
		{
		public:
			static size_t constexpr WINDOW_EVENT_BUFFER_SIZE = 32;

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
			void updateSDLWindowProperties();
			// Note: this method assumes valid OpenGl context
			void setVsyncMode(int8_t mode);

		protected:

			//############# PxeGlAsset API ##################

			void initializeGl() override;
			void uninitializeGl() override;

		private:
			enum class NpWindowFlags : uint8_t
			{
				SIZE_CHANGED		= 0b00000001,
				TITLE_CHANGED		= 0b00000010,
				PRIMARY_WINDOW		= 0b00000100,
				WINDOW_CLOSE		= 0b00001000,
			};

			bool getFlag(NpWindowFlags flag) const;
			void clearFlag(NpWindowFlags flag);
			void setFlag(NpWindowFlags flag, bool value);
			void setFlag(NpWindowFlags flag);

			void* _userData;
			SDL_Window* _sdlWindow;
			ImGuiContext* _guiContext;
			NpScene* _scene;
			PxeCamera* _camera;
			char* _title;
			int32_t _width;
			int32_t _height;
			uint8_t _flags;
			int8_t _vsyncMode;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_WINDOW_H_
