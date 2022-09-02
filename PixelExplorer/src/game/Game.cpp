#include "Game.h"

#include "Logger.h"
#include "gui/MainMenu.h"
#include "chunk/ChunkManager.h"
#include "rendering/ExampleRenderMesh.h"

namespace pixelexplorer::game {
	Game::Game()
	{
		_renderWindow = nullptr;
		Logger::debug(__FUNCTION__" Created Game");
	}

	void Game::start() {
		Logger::debug(__FUNCTION__" Started Game");

		_renderWindow = new rendering::RenderWindow(600, 400, "Pixel Explore");
		chunk::ChunkManager* chunkManager = new chunk::ChunkManager(_renderWindow);
		gui::MainMenu* mainMenu = new gui::MainMenu();
		_renderWindow->addGLRenderObject(mainMenu);

		while (!_renderWindow->shouldClose())
		{
			if (mainMenu != nullptr && mainMenu->getShouldClose()) {
				for (int32_t x = -10; x < 10; ++x)
					for (int32_t y = -10; y < 10; ++y)
						for (int32_t z = -10; z < 10; ++z)
							chunkManager->loadChunk(glm::i32vec3(x,y,z));
				_renderWindow->removeGLRenderObject(mainMenu);
				mainMenu->drop();
				mainMenu = nullptr;
			}

			_renderWindow->drawFrame();
		}

		if (mainMenu != nullptr) {
			mainMenu->drop();
			mainMenu = nullptr;
		}

		chunkManager->unloadAllChunks();
		chunkManager->drop();
		chunkManager = nullptr;

		_renderWindow->drop();
		_renderWindow = nullptr;
	}

	Game::~Game()
	{
		if (_renderWindow != nullptr)
			_renderWindow->drop();
		_renderWindow = nullptr;
		Logger::debug(__FUNCTION__" Destroyed Game");
	}
}