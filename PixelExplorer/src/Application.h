#ifndef PIXELEXPLORER_APPLICATION_H_
#define PIXELEXPLORER_APPLICATION_H_
#include <string>

#include "PxeApplication.h"
#include "ApplicationScene.h"
#include "PxeWindow.h"

namespace pixelexplorer {
	class Application : public pxengine::PxeApplication
	{
	public:
		static Application& getInstance();
		void switchScene(ApplicationScene* scene);
		void quit();
		PXE_NOCOPY(Application);

	protected:
		void onStart() override;
		void onStop() override;
		void onUpdate() override;

	private:
		enum ApplicationState
		{
			INITIALIZING = 0,
			RUNNING,
			STOPPED
		};

		Application();

		pxengine::PxeWindow* _window;
		ApplicationScene* _currentScene;
		ApplicationState _state;
	};
}
#endif // !PIXELEXPLORER_APPLICATION_H_

