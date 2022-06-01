#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdint.h>
#include "Logger.h"
#include "game/Game.h"

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	pixelexplore::game::Game* game = new pixelexplore::game::Game();
	game->start();
	game->drop();
	return 0;
}