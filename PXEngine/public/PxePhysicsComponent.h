#ifndef PXENGINE_PHYSICS_COMPONENT_H_
#define PXENGINE_PHYSICS_COMPONENT_H_
#include "PxeComponent.h"

namespace pxengine {
	class PxeScene;

	class PxePhysicsComponent : public PxeComponent
	{
	public:
		PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(PxePhysicsComponent, PxeComponent);
		virtual ~PxePhysicsComponent();
		PXE_NOCOPY(PxePhysicsComponent);

	protected:
		virtual void onPhysics() = 0;
		PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(PxeComponent);
		PxePhysicsComponent();

	private:
		friend class PxeScene;
	};
}
#endif // !PXENGINE_PHYSICS_COMPONENT_H_
