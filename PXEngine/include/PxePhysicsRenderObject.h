#ifndef PXENGINE_PHYSICS_RENDER_OBJECT_H_
#define PXENGINE_PHYSICS_RENDER_OBJECT_H_
#include "PxeTypes.h"
#include "PxeRenderObject.h"
#include "PxRigidActor.h"
#include "foundation/PxMat44.h"

namespace pxengine {
	// Base class for rendering object with a physx PxRigidActor in world space
	// Note: this object may only be in one scene at once due to physx limitations
	// Note: this assumes it has the only reference to the physx PxRigidActor and calls release on destruction
	class PxePhysicsRenderObject : public PxeRenderObject
	{
	public:
		PxePhysicsRenderObject(PxeRenderMaterial& material, physx::PxRigidActor& physicsActor) : PxeRenderObject(material, PxeRenderPass::PHYSICS_WORLD_SPACE), _physicsActor(physicsActor) { physicsActor.userData = this; }
		virtual ~PxePhysicsRenderObject() { _physicsActor.release(); }
		const physx::PxRigidActor& getPhysicsActor() const { return _physicsActor; }
		physx::PxRigidActor& getPhysicsActor() { return _physicsActor; }

		// Called by the PxeEngine every render to update the position based on the physx actor
		// Note: the physx actor is guaranteed to be read/writable during this call
		virtual void updatePhysicsPosition() {
			positionMatrix = glm::make_mat4(physx::PxMat44(_physicsActor.getGlobalPose()).front());
		}

	private:
		physx::PxRigidActor& _physicsActor;
	};
}

#endif // !PXENGINE_PHYSICS_RENDER_OBJECT_H_
