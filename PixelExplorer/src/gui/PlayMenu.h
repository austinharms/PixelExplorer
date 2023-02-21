#ifndef PIXELEXPLORER_GUI_PLAY_MENU_H_
#define PIXELEXPLORER_GUI_PLAY_MENU_H_
#include "PxeTypes.h"
#include "PxeGUIObject.h"
#include "PxeTexture.h"
#include "imgui.h"
#include "PxeFont.h"

namespace pixelexplorer {
	namespace gui {
		class PlayMenu : public pxengine::PxeGUIObject
		{
		public:
			enum MenuActions : uint8_t
			{
				NONE = 0,
				PLAY = 0x1,
			};

			PlayMenu();
			virtual ~PlayMenu();
			uint8_t getActions();

		protected:
			void onRender() override;

		private:
			enum MenuTextures {
				UNKNOWN = -1,
				BACKGROUND,
				BUTTON_BACKGROUND,
				TEXTURE_COUNT
			};

			const char* TEXTURE_FILES[TEXTURE_COUNT] = {
				"main_menu_background.png",
				"primary_button_background.png",
			};

			pxengine::PxeTexture* _textures[TEXTURE_COUNT];
			pxengine::PxeFont* _titleFont;
			pxengine::PxeFont* _buttonFont;
			uint8_t _actions;
		};
	}
}
#endif // !PIXELEXPLORER_GUI_PLAY_MENU_H_
