#ifndef PIXELEXPLORER_GUI_PLAY_MENU_H_
#define PIXELEXPLORER_GUI_PLAY_MENU_H_
#include "PxeTypes.h"
#include "PxeRenderElement.h"
#include "PxeTexture.h"

namespace pixelexplorer {
	namespace gui {
		class PlayMenu : public pxengine::PxeRenderElement
		{
		public:
			PlayMenu();
			virtual ~PlayMenu();

		protected:
			void onRender() override;

		private:
			pxengine::PxeTexture* _textures[1];
		};
	}
}

#endif // !PIXELEXPLORER_GUI_PLAY_MENU_H_
