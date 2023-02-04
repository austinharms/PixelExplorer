#ifndef PIXELEXPLORER_APPLICATION_H_
#define PIXELEXPLORER_APPLICATION_H_
#include <string>

#include "PxeApplicationInterface.h"
#include "PxeWindow.h"
#include "UpdatableScene.h"
#include "scene/ErrorMenu.h"

namespace pixelexplorer {
	class Application : public pxengine::PxeApplicationInterface
	{
	public:
		static Application& getInstance();
		static void Error();
		static void Error(const char* msg);
		static void Error(const std::string& msg);
		static void ChangeScene(UpdatableScene* scene);

		~Application() = default;
		void onStart() override;
		void onStop() override;
		void onUpdate() override;
		void postGUI(pxengine::PxeWindow& window) override;
		void setError();
		void setError(const char* msg);
		void setError(const std::string& msg);
		void quit();
		void setActiveScene(UpdatableScene* scene);

	private:
		enum ApplicationState
		{
			NONE = 0,
			ERROR,
			MAIN_MENU
		};

		Application();

		pxengine::PxeWindow* _window;
		UpdatableScene* _activeScene;
		scene::ErrorMenu* _errorMenu;
		ApplicationState _state;
		uint64_t _frameCountTimer;
		uint16_t _frameCount;
		uint16_t _lastFrameCount;
	};
}
#endif // !PIXELEXPLORER_APPLICATION_H_

