#ifndef PIXELEXPLORER_TITLE_SCENE_H_
#define PIXELEXPLORER_TITLE_SCENE_H_
#include "PxeTypes.h"
#include "ApplicationScene.h"
#include "TitleScreen.h"

namespace pixelexplorer {
	namespace scenes {
		namespace title {
			class TitleScene : public ApplicationScene
			{
			public:
				static TitleScene* create();
				void onStart(pxengine::PxeWindow& window) override;
				void onStop() override;
				virtual ~TitleScene();
				PXE_NOCOPY(TitleScene);

			private:
				TitleScene(pxengine::PxeScene& scene, TitleScreen& title);

				TitleScreen& _titleScreen;
			};
		}
	}
}
#endif // !PIXELEXPLORER_TITLE_SCENE_H_
