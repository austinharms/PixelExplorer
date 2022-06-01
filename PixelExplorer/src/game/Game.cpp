#include "Game.h"

#include "Logger.h"

namespace pixelexplore::game {
	Game::Game()
	{
		Logger::debug("Created Game Instance");
		_renderWindow = new rendering::RenderWindow(600, 400, "Pixel Explore");
	}

	void Game::start() {
		while (!_renderWindow->shouldClose())
		{
			_renderWindow->drawFrame();
		}
	}

	Game::~Game()
	{
		_renderWindow->drop();
		Logger::debug("Destroyed Game Instance");
	}
}