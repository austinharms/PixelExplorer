#ifndef PXENGINE_PHYSICS_UPDATE_OBJECT_INTERFACE_H
#define PXENGINE_PHYSICS_UPDATE_OBJECT_INTERFACE_H
#include "PxeTypes.h"

namespace pxengine {
	// Interface class managing and updating physx state in a PxeScene
	class PxePhysicsUpdateObjectInterface
	{
	public:
		// This method is called before every scene physics update
		// Note: this requires the PHYSICS_UPDATE flag to be set on the base PxeObject
		virtual void onPhysics() {};
	};
}
#endif // !PXENGINE_PHYSICS_UPDATE_OBJECT_INTERFACE_H
