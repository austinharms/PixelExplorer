#include "Game.h"

#include "Logger.h"
#include "rendering/RenderMesh.h"
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
		_renderWindow->addGUIElement(mainMenu);

		block::Shape* testShape = new block::Shape("./assets/blocks/shapes/default.obj");
		block::RenderShape* testMesh = new block::RenderShape(testShape);
		testShape->drop();
		_renderWindow->addRenderMesh(testMesh);

		while (!_renderWindow->shouldClose())
		{
			if (mainMenu != nullptr && mainMenu->getShouldClose()) {
				_renderWindow->removeGUIElement(mainMenu);
				mainMenu->drop();
				mainMenu = nullptr;
			}

			_renderWindow->drawFrame();
		}

		if (mainMenu != nullptr) {
			_renderWindow->removeGUIElement(mainMenu);
			mainMenu->drop();
			mainMenu = nullptr;
		}

		if (testMesh != nullptr) {
			_renderWindow->removeRenderMesh(testMesh);
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