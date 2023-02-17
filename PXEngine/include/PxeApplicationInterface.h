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
	};
}
#endif // !PXENGINE_APPLICATION_INTERFACE_H_