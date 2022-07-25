#include "Game.h"

#include "Logger.h"
#include "gui/MainMenu.h"
#include "block/Shape.h"
#include "block/RenderShape.h"

namespace pixelexplorer::game {
	Game::Game()
	{
		_renderWindow = nullptr;
		Logger::debug("Created Game");
	}

	void Game::start() {
		Logger::debug("Started Game");

		_renderWindow = new rendering::RenderWindow(600, 400, "Pixel Explore");

		gui::MainMenu* mainMenu = new gui::MainMenu();
		_renderWindow->addGLRenderObject(mainMenu);

		block::Shape* testShape = new block::Shape("./assets/blocks/shapes/default.shape");
		block::RenderShape* testMesh = new block::RenderShape(testShape);
		testShape->drop();
		_renderWindow->addGLRenderObject(testMesh);

		while (!_renderWindow->shouldClose())
		{
			if (mainMenu != nullptr && mainMenu->getShouldClose()) {;
				mainMenu->drop();
				mainMenu = nullptr;
			}

			_renderWindow->drawFrame();
		}

		if (mainMenu != nullptr) {
			mainMenu->drop();
			mainMenu = nullptr;
		}

		if (testMesh != nullptr) {
			testMesh->drop();
			testMesh = nullptr;
		}
	}

	Game::~Game()
	{
		_renderWindow->drop();
		Logger::debug("Destroyed Game");
	}
}