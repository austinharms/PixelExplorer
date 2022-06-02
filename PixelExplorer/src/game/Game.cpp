#include "Game.h"

#include "Logger.h"
#include "rendering/RenderMesh.h"
#include "gui/TestScreen.h"

namespace pixelexplore::game {
	Game::Game()
	{
		_renderWindow = nullptr;
		Logger::debug("Created Game");
	}

	void Game::start() {
		Logger::debug("Started Game");
		_renderWindow = new rendering::RenderWindow(600, 400, "Pixel Explore");
		gui::TestScreen* testScreen = new gui::TestScreen();
		_renderWindow->addGUIElement(testScreen);
		rendering::RenderMesh* testMesh = new rendering::RenderMesh();
		_renderWindow->addRenderMesh(testMesh);
		while (!_renderWindow->shouldClose())
		{
			if (testScreen != nullptr && testScreen->getShouldClose()) {
				_renderWindow->removeGUIElement(testScreen);
				testScreen->drop();
				testScreen = nullptr;
			}

			if (testMesh != nullptr && testScreen != nullptr && testScreen->getRemoveTestMesh()) {
				_renderWindow->removeRenderMesh(testMesh);
				testMesh->drop();
				testMesh = nullptr;
			}

			_renderWindow->drawFrame();
		}

		if (testScreen != nullptr) {
			_renderWindow->removeGUIElement(testScreen);
			testScreen->drop();
			testScreen = nullptr;
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