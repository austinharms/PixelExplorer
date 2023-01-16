#ifndef PXENGINE_APPLICATION_INTERFACE_H_
#define PXENGINE_APPLICATION_INTERFACE_H_
#include "PxeWindow.h"

namespace pxengine {
	// TODO Comment this file
	class PxeApplicationInterface
	{
	public:
		PxeApplicationInterface() = default;
		~PxeApplicationInterface() = default;

		virtual void onStart() = 0;
		virtual void onStop() = 0;
		virtual void onUpdate() = 0;

		virtual void preRender(PxeWindow& window) {}
		virtual void postRender(PxeWindow& window) {}

		virtual void preGUI(PxeWindow& window) {}
		virtual void postGUI(PxeWindow& window) {}

		virtual void prePhysics() {}
		virtual void postPhysics() {}
	};
}
#endif // !PXENGINE_APPLICATION_INTERFACE_H_