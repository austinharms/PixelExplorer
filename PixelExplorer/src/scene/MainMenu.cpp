#include "MainMenu.h"

#include <new>

#include "Application.h"

namespace pixelexplorer {
	namespace scene {
		MainMenu::MainMenu() {
			if (!getScene()) {
				Application::Error("Failed to create MainMenu scene");
				return;
			}

			_playMenu = new(std::nothrow) gui::PlayMenu();
			if (!_playMenu) {
				Application::Error("Failed to create PlayMenu GUI");
				return;
			}

			getScene()->addRenderable(*_playMenu);
		}

		MainMenu::~MainMenu() {
			_playMenu->drop();
		}

		void MainMenu::update() {

		}
	}
}