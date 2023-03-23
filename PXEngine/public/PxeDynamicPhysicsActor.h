#ifndef PXENGINE_DYNAMIC_PHYSICS_ACTOR_H_
#define PXENGINE_DYNAMIC_PHYSICS_ACTOR_H_
#include "PxePhysicsActor.h"
#include "PxRigidDynamic.h"

namespace pxengine {
	class PxeDynamicPhysicsActor : public PxePhysicsActor
	{
	public:
		// Creates and returns a new PxeStaticPhysicsActor instance or nullptr on failure
		static PxeDynamicPhysicsActor* create();
		physx::PxRigidActor& getActor() override;
		physx::PxRigidDynamic& getDynamicActor();
		PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(PxeDynamicPhysicsActor, PxePhysicsActor);
		virtual ~PxeDynamicPhysicsActor();
		PXE_NOCOPY(PxeDynamicPhysicsActor);

	protected:
		PXE_NODISCARD virtual bool checkComponentRequirements(pxengine::PxeObject& object) override;
		virtual void addToObject(pxengine::PxeObject& object) override;
		virtual void removeFromObject(pxengine::PxeObject& object) override;
		virtual void addToScene(pxengine::PxeScene& scene) override;
		virtual void removeFromScene(pxengine::PxeScene& scene) override;
		void onPhysics() override;
		// Note: this will call release on the actor on destruction
		PxeDynamicPhysicsActor(physx::PxRigidDynamic& actor);

	private:
		physx::PxRigidDynamic& _actor;
		glm::mat4 _lastTransform;
	};
}
#endif // !PXENGINE_DYNAMIC_PHYSICS_ACTOR_H_

