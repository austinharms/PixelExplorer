#include "TitleScene.h"

#include "Log.h"
#include "PxeEngine.h"

namespace pixelexplorer {
	namespace scenes {
		namespace title {
			TitleScene* TitleScene::create()
			{
				using namespace pxengine;
				PxeEngine& engine = PxeEngine::getInstance();
				PxeScene* scene = engine.createScene();
				if (!scene) {
					PEX_ERROR("Failed to create TitleScene's PxeScene");
					return nullptr;
				}

				PxeObject* obj = PxeObject::create();
				if (!obj) {
					PEX_ERROR("Failed to create TitleScene's PxeObject");
					return nullptr;
				}

				TitleScreen* screen = TitleScreen::create();
				if (!screen) {
					PEX_ERROR("Failed to create TitleScene's TitleScreen");
					return nullptr;
				}

				if (!obj->addComponent(*screen)) {
					PEX_FATAL("Failed to add ErrorScreen to TitleScene's PxeObject");
					return nullptr;
				}

				if (!scene->addObject(*obj)) {
					PEX_FATAL("Failed to add PxeObject to TitleScene's PxeScene");
					return nullptr;
				}

				obj->drop();
				TitleScene* titleScene = new(std::nothrow) TitleScene(*scene, *screen);
				screen->drop();
				if (!titleScene) {
					PEX_FATAL("Failed to create TitleScene");
					return nullptr;
				}

				return titleScene;
			}

			void TitleScene::onStart(pxengine::PxeWindow& window)
			{
			}

			void TitleScene::onStop()
			{
			}

			TitleScene::TitleScene(pxengine::PxeScene& scene, TitleScreen& title) : ApplicationScene(scene), _titleScreen(title) {
				_titleScreen.grab();
			}

			TitleScene::~TitleScene()
			{
			}
		}
	}
}