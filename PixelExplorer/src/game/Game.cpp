#include "Game.h"

#include "Logger.h"
#include "rendering/RenderWindow.h"
#include "gui/MainMenu.h"
#include "chunk/ChunkManager.h"
#include "rendering/ExampleRenderMesh.h"
#include "block/BlockManifest.h"
#include "rendering/Camera.h"

namespace pixelexplorer::game {
	Game::Game()
	{
		Logger::debug(__FUNCTION__" Created Game");
	}

	void Game::start() {
		Logger::debug(__FUNCTION__" Started Game");
		rendering::Camera* camera = new rendering::Camera();
		rendering::RenderWindow* renderWindow = new rendering::RenderWindow(600, 400, "Pixel Explore", camera);
		block::BlockManifest* blockManifest = new block::BlockManifest();
		blockManifest->load();
		chunk::ChunkManager* chunkManager = new chunk::ChunkManager(renderWindow, blockManifest);
		gui::MainMenu* mainMenu = new gui::MainMenu();
		renderWindow->addGLRenderObject(mainMenu);

		while (!renderWindow->shouldClose())
		{
			if (mainMenu != nullptr && mainMenu->getShouldClose()) {
				uint16_t chunkCount = 3;
				for (int32_t x = -chunkCount; x < chunkCount; ++x)
					for (int32_t y = -chunkCount; y < chunkCount; ++y)
						for (int32_t z = -chunkCount; z < chunkCount; ++z)
							chunkManager->loadChunk(glm::i32vec3(x,y,z));
				renderWindow->removeGLRenderObject(mainMenu);
				mainMenu->drop();
				mainMenu = nullptr;
			}

			renderWindow->drawFrame();
		}

		if (mainMenu != nullptr) {
			mainMenu->drop();
			mainMenu = nullptr;
		}

		chunkManager->unloadAllChunks();
		chunkManager->drop();
		chunkManager = nullptr;

		blockManifest->unload();
		blockManifest->drop();
		blockManifest = nullptr;

		renderWindow->drop();
		renderWindow = nullptr;
	}

	Game::~Game()
	{
		Logger::debug(__FUNCTION__" Destroyed Game");
	}
}