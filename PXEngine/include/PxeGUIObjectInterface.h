#ifndef PXENGINE_GUI_OBJECT_INTERFACE_H
#define PXENGINE_GUI_OBJECT_INTERFACE_H
#include "PxeTypes.h"
#include "glm/mat4x4.hpp"

namespace pxengine {
	// Interface class for rendering GUI using a PxeScene
	class PxeGUIObjectInterface
	{
	public:
		// This method should draw GUI objects to the current GUI context
		// Note: this requires the GUI_UPDATE flag to be set on the base PxeObject
		virtual void onGUI() = 0;
	};
}
#endif // !PXENGINE_GUI_OBJECT_INTERFACE_H
