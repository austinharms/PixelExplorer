#include "ErrorScene.h"

#include <new>

#include "Log.h"
#include "PxeEngine.h"

namespace pixelexplorer {
	namespace scenes {
		namespace error {
			ErrorScene* ErrorScene::create()
			{
				using namespace pxengine;
				PxeEngine& engine = PxeEngine::getInstance();
				PxeScene* scene = engine.createScene();
				if (!scene) {
					PEX_FATAL("Failed to create ErrorScene's PxeScene");
					return nullptr;
				}

				PxeObject* obj = PxeObject::create();
				if (!obj) {
					PEX_FATAL("Failed to create ErrorScene's PxeObject");
					return nullptr;
				}

				PxeGuiRenderProperties* renProps = PxeGuiRenderProperties::getInstance();
				if (!renProps) {
					PEX_FATAL("Failed to get PxeGuiRenderProperties for ErrorScreen");
					return nullptr;
				}

				ErrorScreen* screen = new(std::nothrow) ErrorScreen(*renProps);
				renProps->drop();
				if (!screen) {
					PEX_FATAL("Failed to create ErrorScene's ErrorScreen");
					return nullptr;
				}

				if (!obj->addComponent(*screen)) {
					PEX_FATAL("Failed to add ErrorScreen to ErrorScene's PxeObject");
					return nullptr;
				}

				if (!scene->addObject(*obj)) {
					PEX_FATAL("Failed to add PxeObject to ErrorScene's PxeScene");
					return nullptr;
				}

				obj->drop();
				ErrorScene* errScene = new(std::nothrow) ErrorScene(*scene, *screen);
				if (!errScene) {
					PEX_FATAL("Failed to create ErrorScene");
					return nullptr;
				}

				return errScene;
			}

			ErrorScene::ErrorScene(pxengine::PxeScene& scene, ErrorScreen& errScreen) : 
				ApplicationScene(scene), _errorScreen(errScreen) { }

			ErrorScene::~ErrorScene()
			{
				_errorScreen.drop();
			}

			void ErrorScene::onStart(pxengine::PxeWindow& window)
			{

			}

			void ErrorScene::onStop()
			{
				
			}

			void ErrorScene::setMessage(const char* msg)
			{
				_errorScreen.setMessage(msg);
			}
		}
	}
}