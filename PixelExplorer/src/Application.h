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

		~Application() = default;
		void onStart() override;
		void onStop() override;
		void onUpdate() override;
		void setError();
		void setError(const char* msg);
		void setError(const std::string& msg);
		void quit();

	private:
		enum ApplicationState
		{
			NONE = 0,
			ERROR,
			MAIN_MENU
		};

		Application();
		void setActiveScene(UpdatableScene* scene);

		pxengine::PxeWindow* _window;
		UpdatableScene* _activeScene;
		scene::ErrorMenu* _errorMenu;
		ApplicationState _state;
	};
}
#endif // !PIXELEXPLORER_APPLICATION_H_

