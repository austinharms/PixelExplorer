#include "TitleScene.h"

#include <new>

#include "Log.h"
#include "PxeEngine.h"
#include "Application.h"
#include "scenes/generation/TerrainGenerationTest.h"

namespace pixelexplorer {
	namespace scenes {
		namespace title {
			TitleScene::TitleScene() {
				using namespace pxengine;
				_titleScreen = new(std::nothrow) TitleScreen();
				if (!_titleScreen) { PEX_FATAL("Failed to allocate TitleScene's TitleScreen"); }
				PxeObject* obj = PxeObject::create();
				if (!obj) { PEX_FATAL("Failed to create TitleScene's PxeObject"); }
				if (!obj->addComponent(*_titleScreen)) { PEX_FATAL("Failed to add ErrorScreen to TitleScene's PxeObject"); }
				if (!getScene().addObject(*obj)) { PEX_FATAL("Failed to add PxeObject to TitleScene's PxeScene"); }
				obj->drop();
			}

			TitleScene::~TitleScene()
			{
				_titleScreen->drop();
			}

			void TitleScene::onStart(pxengine::PxeWindow& window)
			{
			}

			void TitleScene::onStop()
			{
			}

			void TitleScene::onUpdate()
			{
				if (_titleScreen->getActions() & TitleScreen::PLAY) {
					generation::TerrainGenerationTest* scene = new(std::nothrow) generation::TerrainGenerationTest();
					if (!scene) {
						PEX_FATAL("Failed to allocate TerrainGenerationTest");
					}

					Application::getInstance().switchScene(scene);
				}
			}
		}
	}
}