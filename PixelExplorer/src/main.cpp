#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdint.h>
#include <thread>

#include "Logger.h"
#include "game/Game.h"

void playGame() {
	pixelexplorer::game::Game* game = new pixelexplorer::game::Game();
	game->start();
	game->drop();
}

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	std::thread a(playGame);
	//std::thread b(playGame);
	//std::thread c(playGame);
	a.join();
	//b.join();
	//c.join();
	//playGame();
	return 0;
}