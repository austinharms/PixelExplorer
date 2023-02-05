#ifndef PXENGINE_GUI_OBJECT_H_
#define PXENGINE_GUI_OBJECT_H_
#include "PxeTypes.h"
#include "PxeObject.h"
#include "PxeGUIObjectInterface.h"

namespace pxengine {
	// base class for GUI rendering
	class PxeGUIObject : public PxeObject, public PxeGUIObjectInterface
	{
	public:
		inline PxeGUIObject(PxeObjectFlags flags = PxeObjectFlags::GUI_UPDATE) : PxeObject(flags) {}
		virtual ~PxeGUIObject() = default;

		// This method should draw GUI objects to the current GUI context
		// Note: you can assume a valid ImGui context and frame
		// Note: this requires the GUI_UPDATE flag to be set (flag is set by default)
		virtual void onGUI() override = 0;
	};
}
#endif // !PXENGINE_GUI_OBJECT_H_
