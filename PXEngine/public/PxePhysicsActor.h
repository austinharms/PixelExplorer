#ifndef PXENGINE_PHYSICS_ACTOR_H_
#define PXENGINE_PHYSICS_ACTOR_H_
#include <list>
#include <mutex>

#include "PxePhysicsComponent.h"
#include "PxePhysicsShape.h"

namespace pxengine {
	// TODO Comment file
	class PxePhysicsActor : public PxePhysicsComponent
	{
	public:
		virtual ~PxePhysicsActor();
		PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(PxePhysicsActor, PxePhysicsComponent);

	protected:
		PXE_NODISCARD virtual bool checkComponentRequirements(pxengine::PxeObject& object) override;
		virtual void addToObject(pxengine::PxeObject& object) override;
		virtual void removeFromObject(pxengine::PxeObject& object) override;
		virtual void addToScene(pxengine::PxeScene& scene) override;
		virtual void removeFromScene(pxengine::PxeScene& scene) override;
		virtual void onPhysics() override = 0;
		virtual physx::PxRigidActor& getActor() = 0;
		PxePhysicsActor();

	private:
		friend class PxePhysicsShape;
		void addShape(PxePhysicsShape& shape);
		void removeShape(PxePhysicsShape& shape);
	};
}
#endif // !PXENGINE_PHYSICS_ACTOR_H_
