#include "Game.h"

#include <string>
#include <filesystem>

#include "FPSCamera.h"
#include "common/Logger.h"
#include "common/OSHelpers.h"
#include "engine/rendering/RenderWindow.h"
#include "gui/MainMenu.h"
#include "chunk/ChunkManager.h"
#include "world/WorldDetails.h"
#include "world/World.h"
#include "engine/Camera.h"
#include "player/Player.h"
#include "GLFW/glfw3.h"
#include "engine/input/InputAction.h"
#include "engine/input/InputSource.h"
#include "engine/input/InputManager.h"

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


		// create world details, this should be updated to load from files
		std::filesystem::path worldPath = OSHelper::getPersistentPath("TEST_WORLD");
		world::WorldDetails* worldDetails = new(std::nothrow) world::WorldDetails(worldPath, "TEST_WORLD");
		if (worldDetails == nullptr) {
			Logger::error(__FUNCTION__" failed to load world details");
			return;
		}

		// load world from details
		world::World* world = new(std::nothrow) world::World(*worldDetails, *_window);
		worldDetails->drop();
		worldDetails = nullptr;
		if (world == nullptr) {
			// should this be fatal, we can technicaly just return to main menu
			Logger::error(__FUNCTION__" failed to allocate world");
			return;
		}

		// store the current window camera to restore it later
		engine::rendering::CameraInterface* oldCamera = _window->getCamera();
		if (oldCamera) oldCamera->grab();
		FPSCamera* freeViewCam = new(std::nothrow) FPSCamera(_window->getInputManager());
		bool freeViewCamActive = false;
		InputAction* switchCameraAction = _window->getInputManager().getOrCreateAction("switch camera", InputSource{ InputSource::InputSourceType::KEYBOARD, glfwGetKeyScancode(GLFW_KEY_C), false, false });
		//camera->setPosition(glm::vec3(0, 0, -50));
		//_window->setCamera(camera);

		// reset delta timer as we just loaded in lots of data that presumably took a long time
		_window->resetDeltaTimer();
		while (!_window->shouldClose())
		{
			_window->drawFrame();
			if (switchCameraAction->getValue()) {
				freeViewCamActive = !freeViewCamActive;
				if (freeViewCamActive && freeViewCam == nullptr)
					freeViewCamActive = false;

				if (freeViewCamActive) {
					_window->setCamera(freeViewCam);
				}
				else {
					_window
				}
			}

			camera->update(_window->getDeltaTime());
			world->update();
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