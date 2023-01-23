#ifndef PIXELEXPLORER_GUI_ERROR_SCREEN_H_
#define PIXELEXPLORER_GUI_ERROR_SCREEN_H_
#include <string>

#include "PxeTypes.h"
#include "PxeRenderElement.h"
#include "PxeTexture.h"

namespace pixelexplorer {
	namespace gui {
		class ErrorScreen : public pxengine::PxeRenderElement
		{
		public:
			ErrorScreen() = default;
			virtual ~ErrorScreen() = default;
			void setMessage(const char* msg);
			void setMessage(const std::string& msg);

		protected:
			void onRender() override;

		private:
			std::string _errorMessage;
		};
	}
}

#endif // !PIXELEXPLORER_GUI_ERROR_SCREEN_H_