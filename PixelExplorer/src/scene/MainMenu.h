#ifndef PIXELEXPLORER_SCENE_MAIN_MENU_H_
#define PIXELEXPLORER_SCENE_MAIN_MENU_H_
#include "UpdatableScene.h"
#include "gui/PlayMenu.h"

namespace pixelexplorer {
	namespace scene {
		class MainMenu : public UpdatableScene
		{
		public:
			MainMenu();
			virtual ~MainMenu();
			void update() override;

		private:
			gui::PlayMenu* _playMenu;
		};
	}
}
#endif // !PIXELEXPLORER_SCENE_MAIN_MENU_H_
