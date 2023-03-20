#ifndef PXENGINE_STATIC_PHYSICS_ACTOR_H_
#define PXENGINE_STATIC_PHYSICS_ACTOR_H_
#include "PxePhysicsActor.h"
#include "PxRigidStatic.h"

namespace pxengine {
	class PxeStaticPhysicsActor : public PxePhysicsActor
	{
	public:
		// Creates and returns a new PxeStaticPhysicsActor instance or nullptr on failure
		static PxeStaticPhysicsActor* create();
		virtual ~PxeStaticPhysicsActor();
		PXE_NOCOPY(PxeStaticPhysicsActor);
		PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(PxeStaticPhysicsActor, PxePhysicsActor);

	protected:
		PXE_NODISCARD virtual bool checkComponentRequirements(pxengine::PxeObject& object) override;
		virtual void addToObject(pxengine::PxeObject& object) override;
		virtual void removeFromObject(pxengine::PxeObject& object) override;
		virtual void addToScene(pxengine::PxeScene& scene) override;
		virtual void removeFromScene(pxengine::PxeScene& scene) override;
		void onPhysics() override;
		physx::PxRigidActor& getActor() override;
		// Note: this will call release on the actor on destruction
		PxeStaticPhysicsActor(physx::PxRigidStatic& actor);

	private:
		physx::PxRigidStatic& _actor;
		glm::mat4 _lastTransform;
	};
}
#endif // !PXENGINE_STATIC_PHYSICS_ACTOR_H_