#ifndef PXENGINE_DYNAMIC_PHYSICS_RENDER_OBJECT_H_
#define PXENGINE_DYNAMIC_PHYSICS_RENDER_OBJECT_H_
#include "PxeTypes.h"
#include "PxeRenderObject.h"
#include "PxRigidActor.h"
#include "foundation/PxMat44.h"

namespace pxengine {
	// Base class for rendering object with a physx PxRigidActor that changes position in world space
	// Note: this object may only be in one scene at once due to physx limitations
	// Note: this assumes it has the only reference to the physx PxRigidActor and calls release on destruction
	class PxeDynamicPhysicsRenderObject : public PxeRenderObject
	{
	public:
		PxeDynamicPhysicsRenderObject(PxeRenderMaterial& material, physx::PxRigidActor* physicsActor = nullptr) : PxeRenderObject(material, PxeRenderPass::DYNAMIC_PHYSICS_WORLD_SPACE) { setPhysicsActor(physicsActor); }
		virtual ~PxeDynamicPhysicsRenderObject() { if (_physicsActor) _physicsActor->release(); }
		const physx::PxRigidActor* getPhysicsActor() const { return _physicsActor; }
		physx::PxRigidActor* getPhysicsActor() { return _physicsActor; }

		// Called by the PxeEngine every render to update the position based on the physx actor
		// Note: the physx actor is guaranteed to be read/writable during this call
		virtual void updatePhysicsPosition() {
			if (_physicsActor)
				positionMatrix = glm::make_mat4(physx::PxMat44(_physicsActor->getGlobalPose()).front());
		}

	protected:
		void setPhysicsActor(physx::PxRigidActor* physicsActor) {
			_physicsActor = physicsActor;
			if (_physicsActor) {
				physicsActor->userData = this;
			}
		}

	private:
		physx::PxRigidActor* _physicsActor;
	};
}

#endif // !PXENGINE_STATIC_PHYSICS_RENDER_OBJECT_H_
