#ifndef PIXELEXPLORER_GUI_PAUSE_MENU_H_
#define PIXELEXPLORER_GUI_PAUSE_MENU_H_
#include "PxeTypes.h"
#include "PxeGuiComponent.h"
#include "PxeTexture.h"
#include "imgui.h"
#include "PxeFont.h"

namespace pixelexplorer {
	namespace scenes {
		namespace generation {
			class PauseMenu : public pxengine::PxeGuiComponent
			{
			public:
				enum MenuActions : uint8_t
				{
					NONE = 0,
					PLAY = 0x1,
				};

				uint8_t getActions();
				void setEnabled(bool enabled);
				bool getEnabled() const;
				PauseMenu();
				virtual ~PauseMenu();

			protected:
				void onRender() override;

			private:
				enum MenuTextures {
					UNKNOWN = -1,
					BACKGROUND,
					BUTTON_BACKGROUND,
					TEXTURE_COUNT
				};

				pxengine::PxeTexture* _textures[TEXTURE_COUNT];
				pxengine::PxeFont* _menuFont;
				uint8_t _actions;
				bool _enabled;
			};
		}
	}
}
#endif // !PIXELEXPLORER_GUI_PAUSE_MENU_H_
