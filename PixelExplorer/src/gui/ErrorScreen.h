#ifndef PIXELEXPLORER_GUI_ERROR_SCREEN_H_
#define PIXELEXPLORER_GUI_ERROR_SCREEN_H_
#include <string>

#include "PxeTypes.h"
#include "PxeGUIObject.h"
#include "PxeTexture.h"
#include "PxeFont.h"

namespace pixelexplorer {
	namespace gui {
		class ErrorScreen : public pxengine::PxeGUIObject
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
			pxengine::PxeFont* _font;
		};
	}
}

#endif // !PIXELEXPLORER_GUI_ERROR_SCREEN_H_