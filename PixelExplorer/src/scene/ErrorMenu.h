#ifndef PIXELEXPLORER_SCENE_ERROR_SCREEN_H_
#define PIXELEXPLORER_SCENE_ERROR_SCREEN_H_
#include <string>

#include "UpdatableScene.h"
#include "gui/ErrorScreen.h"

namespace pixelexplorer {
	namespace scene {
		class ErrorMenu : public UpdatableScene
		{
		public:
			ErrorMenu();
			virtual ~ErrorMenu();
			void setMessage(const char* msg);
			void setMessage(const std::string& msg);
			void update() override;

		private:
			gui::ErrorScreen* _errorScreen;
		};
	}
}
#endif // !PIXELEXPLORER_SCENE_MAIN_MENU_H_
