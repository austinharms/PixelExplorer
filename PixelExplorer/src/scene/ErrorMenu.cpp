#include "ErrorMenu.h"

#include <new>

#include "Log.h"

namespace pixelexplorer {
	namespace scene {
		ErrorMenu::ErrorMenu() {
			_errorScreen = nullptr;
			if (!getScene()) {
				PEX_FATAL("Failed to create error PxeScene");
				return;
			}

			_errorScreen = new(std::nothrow) gui::ErrorScreen();
			if (!_errorScreen) {
				PEX_FATAL("Failed to create ErrorScreen");
				return;
			}

			_errorScreen->setMessage("An unknown error occurred");
			getScene()->addObject(*_errorScreen);
		}

		ErrorMenu::~ErrorMenu()
		{
			_errorScreen->drop();
		}

		void ErrorMenu::setMessage(const char* msg)
		{
			_errorScreen->setMessage(msg);
		}

		void ErrorMenu::setMessage(const std::string& msg)
		{
			_errorScreen->setMessage(msg);
		}

		void ErrorMenu::update() { }
	}
}