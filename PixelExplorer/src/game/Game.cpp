#include "Game.h"

#include "Logger.h"
#include "rendering/RenderMesh.h"
#include "gui/MainMenu.h"

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
		//rendering::RenderMesh* testMesh = new rendering::RenderMesh();
		//_renderWindow->addRenderMesh(testMesh);
		while (!_renderWindow->shouldClose())
		{
			if (mainMenu != nullptr && mainMenu->getShouldClose()) {
				_renderWindow->removeGUIElement(mainMenu);
				mainMenu->drop();
				mainMenu = nullptr;
			}

			//if (testMesh != nullptr && testScreen != nullptr && testScreen->getRemoveTestMesh()) {
			//	_renderWindow->removeRenderMesh(testMesh);
			//	testMesh->drop();
			//	testMesh = nullptr;
			//}

			_renderWindow->drawFrame();
		}

		if (mainMenu != nullptr) {
			_renderWindow->removeGUIElement(mainMenu);
			mainMenu->drop();
			mainMenu = nullptr;
		}

		//if (testMesh != nullptr) {
		//	_renderWindow->removeRenderMesh(testMesh);
		//	testMesh->drop();
		//	testMesh = nullptr;
		//}
	}

	Game::~Game()
	{
		_renderWindow->drop();
		Logger::debug("Destroyed Game");
	}
}