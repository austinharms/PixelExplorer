#ifndef PIXELEXPLORER_SCENES_ERROR_SCENE_H_
#define PIXELEXPLORER_SCENES_ERROR_SCENE_H_
#include "PxeTypes.h"
#include "ApplicationScene.h"
#include "ErrorScreen.h"

namespace pixelexplorer {
	namespace scenes {
		namespace error {
			class ErrorScene : public ApplicationScene
			{
			public:
				static ErrorScene* create();

				void onStart(pxengine::PxeWindow& window) override;
				void onStop() override;
				void setMessage(const char* msg);
				virtual ~ErrorScene();
				PXE_NOCOPY(ErrorScene);

			private:
				ErrorScene(pxengine::PxeScene& scene, ErrorScreen& errScreen);
				ErrorScreen& _errorScreen;
			};
		}
	}
}
#endif // !PIXELEXPLORER_SCENES_ERROR_SCENE_H_
