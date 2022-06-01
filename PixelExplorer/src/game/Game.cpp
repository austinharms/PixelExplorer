#include "Game.h"

#include "Logger.h"
#include "rendering/RenderMesh.h"

namespace pixelexplore::game {
	Game::Game()
	{
		_renderWindow = nullptr;
		Logger::debug("Created Game");
	}

	void Game::start() {
		Logger::debug("Started Game");
		_renderWindow = new rendering::RenderWindow(600, 400, "Pixel Explore");
		rendering::RenderMesh* testMesh = new rendering::RenderMesh();
		_renderWindow->addRenderMesh(testMesh);
		testMesh->drop();
		while (!_renderWindow->shouldClose())
		{
			_renderWindow->drawFrame();
		}
	}

	Game::~Game()
	{
		_renderWindow->drop();
		Logger::debug("Destroyed Game");
	}
}