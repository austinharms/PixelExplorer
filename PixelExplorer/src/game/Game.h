#include <stdint.h>

#include "common/RefCount.h"
#include "engine/rendering/RenderWindow.fwd.h"
#include "gui/MainMenu.h"

#ifndef PIXELEXPLORE_GAME_GAME_H_
#define PIXELEXPLORE_GAME_GAME_H_
namespace pixelexplorer::game {
	class Game : public RefCount
	{
	public:
		Game();
		virtual ~Game();
		void play();

	private:
		engine::rendering::RenderWindow* _window;

		gui::MainMenu::MainMenuAction playMainMenu();
		void playGame();
	};
}
#endif // !PIXELEXPLORE_GAME_H_
