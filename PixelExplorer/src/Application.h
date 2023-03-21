#ifndef PIXELEXPLORER_APPLICATION_H_
#define PIXELEXPLORER_APPLICATION_H_
#include <string>

#include "PxeApplication.h"
#include "ApplicationScene.h"
#include "PxeWindow.h"
#include "scenes/error/ErrorScene.h"

namespace pixelexplorer {
	class Application : public pxengine::PxeApplication
	{
	public:
		static Application& getInstance();
		static void Error();
		static void Error(const std::string& msg);
		static void Error(const char* msg);

		void switchScene(ApplicationScene& scene);
		void quit();
		~Application();
		PXE_NOCOPY(Application);

	protected:
		void onStart() override;
		void onStop() override;
		void onUpdate() override;
		void postUpdate() override;
		void prePhysics() override;
		void postPhysics() override;

	private:
		enum ApplicationState
		{
			INITIALIZING = 0,
			RUNNING,
			STOPPED,
			ERROR,
		};

		Application();

		pxengine::PxeWindow* _window;
		ApplicationScene* _currentScene;
		scenes::error::ErrorScene* _errorScene;
		ApplicationState _state;
	};
}
#endif // !PIXELEXPLORER_APPLICATION_H_

