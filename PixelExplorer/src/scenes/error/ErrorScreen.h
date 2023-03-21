#ifndef PIXELEXPLORER_GUI_ERROR_SCREEN_H_
#define PIXELEXPLORER_GUI_ERROR_SCREEN_H_
#include "PxeTypes.h"
#include "PxeRenderComponent.h"
#include "PxeGuiRenderProperties.h"
#include "PxeTexture.h"
#include "PxeFont.h"

namespace pixelexplorer {
	namespace scenes {
		namespace error {
			class ErrorScreen : public pxengine::PxeRenderComponent
			{
			public:
				void setMessage(const char* msg);
				ErrorScreen(pxengine::PxeGuiRenderProperties& renderProperties);
				virtual ~ErrorScreen();
				PXE_NOCOPY(ErrorScreen);
				PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(ErrorScreen, pxengine::PxeRenderComponent);

			protected:
				void onRender() override;
				PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(pxengine::PxeRenderComponent);

			private:
				char* _msg;
				pxengine::PxeFont* _font;
			};
		}
	}
}

#endif // !PIXELEXPLORER_GUI_ERROR_SCREEN_H_