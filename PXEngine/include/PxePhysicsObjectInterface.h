#ifndef PXENGINE_PHYSICS_OBJECT_INTERFACE_H
#define PXENGINE_PHYSICS_OBJECT_INTERFACE_H
#include "PxeTypes.h"
#include "PxActor.h"

namespace pxengine {
	// Interface class managing physx actors in a PxeScene
	// Note: if the PxeObject inherits this class the PxeObject can only be in one PxeScene at a time due to a limitation of the physx actor
	class PxePhysicsObjectInterface
	{
	public:
		virtual PXE_NODISCARD physx::PxActor* getPhysicsActor() const = 0;
	};
}
#endif // !PXENGINE_PHYSICS_OBJECT_INTERFACE_H
