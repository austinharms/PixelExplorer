#ifndef PIXELEXPLORER_SCENES_TITLE_SCREEN_H_
#define PIXELEXPLORER_SCENES_TITLE_SCREEN_H_
#include "PxeTypes.h"
#include "PxeRenderComponent.h"
#include "PxeTexture.h"
#include "PxeFont.h"
#include "PxeGuiRenderProperties.h"

namespace pixelexplorer {
	namespace scenes {
		namespace title {
			class TitleScreen : public pxengine::PxeRenderComponent
			{
			public:
				typedef uint8_t TitleScreenAction;
				enum TITLE_SCREEN_ACTIONS : TitleScreenAction
				{
					NONE = 0,
					PLAY = 0x01
				};

				static TitleScreen* create();

				TitleScreenAction getActions();
				TitleScreenAction peekActions() const;
				void clearActions();
				virtual ~TitleScreen();
				PXE_NOCOPY(TitleScreen);
				PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(TitleScreen, pxengine::PxeRenderComponent);

			protected:
				void onRender() override;
				PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(pxengine::PxeRenderComponent);

			private:
				enum TEXTURES
				{
					INVALID_TEXTURE = -1,
					BACKGROUND,
					BUTTON_BACKGROUND,
					TEXTURE_COUNT
				};

				enum FONTS
				{
					INVALID_FONT = -1,
					TITLE,
					BUTTON,
					FONT_COUNT
				};

				TitleScreen(pxengine::PxeGuiRenderProperties& renderProperties);
				pxengine::PxeTexture* _textures[TEXTURE_COUNT];
				pxengine::PxeFont* _fonts[FONT_COUNT];
				TitleScreenAction _actions;
				bool _loaded;
			};
		}
	}
}
#endif // !PIXELEXPLORER_SCENES_TITLE_SCREEN_H_
