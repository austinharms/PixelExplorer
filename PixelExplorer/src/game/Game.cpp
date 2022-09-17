#include "Game.h"

#include <string>

#include "FPSCamera.h"
#include "common/Logger.h"
#include "engine/rendering/RenderWindow.h"
#include "gui/MainMenu.h"
#include "chunk/ChunkManager.h"
#include "world/WorldDetails.h"
#include "world/World.h"
#include "engine/Camera.h"
#include "GLFW/glfw3.h"

namespace pixelexplorer::game {
	Game::Game()
	{
		engine::Camera* camera = new engine::Camera();
		_window = new(std::nothrow) engine::rendering::RenderWindow(600, 400, "Pixel Explore", camera);
		if (_window == nullptr) Logger::fatal(__FUNCTION__" failed to allocate RenderWindow");
		camera->drop();
		Logger::debug(__FUNCTION__" Created Game");
	}

	void Game::play() {
		Logger::debug(__FUNCTION__" Play Game");
		while (true)
		{
			using gui::MainMenu;
			gui::MainMenu::MainMenuAction action = playMainMenu();
			switch (action)
			{
			case MainMenu::MainMenuAction::PLAY:
				playGame();
				break;
			case MainMenu::MainMenuAction::CLOSE:
			default:
				_window->setShouldClose();
				return;
			}
		}
	}

	gui::MainMenu::MainMenuAction Game::playMainMenu()
	{
		using gui::MainMenu;
		Logger::debug(__FUNCTION__" Main Menu");
		gui::MainMenu& mainMenu = *(new gui::MainMenu());
		_window->addGLRenderObject(mainMenu);
		MainMenu::MainMenuAction returnAction = MainMenu::MainMenuAction::CLOSE;
		// reset delta timer as we don't know how long it's been since the last frame
		_window->resetDeltaTimer();
		while (!_window->shouldClose())
		{
			_window->drawFrame();
			if (mainMenu.getMenuAction() != MainMenu::MainMenuAction::NONE) {
				returnAction = mainMenu.getMenuAction();
				break;
			}
		}

		_window->resetShouldClose();
		_window->removeGLRenderObject(mainMenu);
		// draw one more frame to terminate/free mainMenu GLObject, (not needed as main menu was removed on the main thread)
		_window->drawFrame();
		if (!mainMenu.drop())
			Logger::warn(__FUNCTION__" main menu GLObject not dropped, make sure all other references to the menu are dropped");
		return returnAction;
	}

	void Game::playGame()
	{
		using namespace engine::input;
		Logger::debug(__FUNCTION__" Playing Game");

		// store the current window camera to restore it later
		engine::rendering::CameraInterface* oldCamera = _window->getCamera();
		if (oldCamera) oldCamera->grab();
		FPSCamera* camera = new FPSCamera(_window->getInputManager());
		camera->setPosition(glm::vec3(0, 0, -50));
		_window->setCamera(camera);

		// create world details, this should be updated to load from files
		world::WorldDetails* worldDetails = new(std::nothrow) world::WorldDetails("", "TEST_WORLD");
		if (worldDetails == nullptr) {
			Logger::error(__FUNCTION__" failed to load world details");
			_window->setCamera(oldCamera);
			if (oldCamera) {
				oldCamera->drop();
				oldCamera = nullptr;
			}

			camera->drop();
			camera = nullptr;
			return;
		}

		// load world from details
		world::World* world = new world::World(*worldDetails, *_window);
		worldDetails->drop();
		worldDetails = nullptr;
		chunk::ChunkManager* chunkManager = world->getChunkManager(0);
		uint16_t chunkCount = 3;
		for (int32_t x = -chunkCount; x < chunkCount; ++x)
			for (int32_t y = -chunkCount; y < chunkCount; ++y)
				for (int32_t z = -chunkCount; z < chunkCount; ++z)
					chunkManager->loadChunk(glm::i32vec3(x, y, z));
		// reset delta timer as we just loaded in lots of data that presumably took a long time
		_window->resetDeltaTimer();
		while (!_window->shouldClose())
		{
			_window->drawFrame();
			camera->update(_window->getDeltaTime());
		}

		_window->resetShouldClose();
		world->drop();
		world = nullptr;
		// draw one more frame to terminate/free all the now unloaded chunk render meshes, (currently not needed as chunks are removed on the main thread)
		_window->drawFrame();

		// restore the original camera
		_window->setCamera(oldCamera);
		if (oldCamera) {
			oldCamera->drop();
			oldCamera = nullptr;
		}

		camera->drop();
		camera = nullptr;
	}

	Game::~Game()
	{
		if (!_window->drop())
			Logger::warn(__FUNCTION__" Game RenderWindow not dropped, make sure all other references to the window are dropped");
		_window = nullptr;
		Logger::debug(__FUNCTION__" Destroyed Game");
	}
}