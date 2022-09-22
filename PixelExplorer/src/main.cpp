#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdint.h>
#include <thread>

#include "common/Logger.h"
#include "game/Game.h"

void playGame() {
	using namespace pixelexplorer;
	using namespace pixelexplorer::game;
	Game* game = new(std::nothrow) Game();
	if (game == nullptr)
		Logger::fatal(__FUNCTION__" failed to allocate Game");
	game->play();
	if (!game->drop())
		Logger::warn(__FUNCTION__" Game not dropped, make sure all other references to the Game are dropped");
}

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//std::thread a(playGame);
	//std::thread b(playGame);
	//std::thread c(playGame);
	//a.join();
	//b.join();
	//c.join();
	playGame();
	return 0;
}