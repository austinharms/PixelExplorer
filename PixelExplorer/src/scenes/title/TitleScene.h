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
				void onStart(pxengine::PxeWindow& window) override;
				void onStop() override;
				void onUpdate() override;
				TitleScene();
				virtual ~TitleScene();
				PXE_NOCOPY(TitleScene);

			private:
				TitleScreen* _titleScreen;
			};
		}
	}
}
#endif // !PIXELEXPLORER_TITLE_SCENE_H_
