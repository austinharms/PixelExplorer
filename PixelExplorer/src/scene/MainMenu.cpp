#include "MainMenu.h"

#include <new>

#include "Application.h"
#include "TerrainGenerationTest.h"

namespace pixelexplorer {
	namespace scene {
		MainMenu::MainMenu() {
			_playMenu = nullptr;
			if (!getScene()) {
				Application::Error("Failed to create Main Menu scene");
				return;
			}

			_playMenu = new(std::nothrow) gui::PlayMenu();
			if (!_playMenu) {
				Application::Error("Failed to create Play Menu GUI");
				return;
			}

			getScene()->addObject(*_playMenu);
		}

		MainMenu::~MainMenu() {
			_playMenu->drop();
		}

		void MainMenu::update() {
			if (_playMenu->getActions() & gui::PlayMenu::PLAY) {
				TerrainGenerationTest* test = new(std::nothrow) TerrainGenerationTest();
				if (!test) {
					Application::Error("Failed to create Terrain Generation Test scene");
					return;
				}

				Application::ChangeScene(test);
				test->drop();
				return;
			}
		}
	}
}