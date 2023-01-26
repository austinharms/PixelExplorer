#ifndef PXENGINE_STATIC_PHYSICS_RENDER_OBJECT_H_
#define PXENGINE_STATIC_PHYSICS_RENDER_OBJECT_H_
#include "PxeTypes.h"
#include "PxeRenderObject.h"
#include "PxRigidActor.h"
#include "foundation/PxMat44.h"

namespace pxengine {
	// Base class for rendering object with a physx PxActor in world space
	// Note: this object may only be in one scene at once due to physx limitations
	// Note: this assumes it has the only reference to the physx PxActor and calls release on destruction
	class PxeStaticPhysicsRenderObject : public PxeRenderObject
	{
	public:
		PxeStaticPhysicsRenderObject(PxeRenderMaterial& material, physx::PxActor* physicsActor = nullptr) : PxeRenderObject(material, PxeRenderPass::STATIC_PHYSICS_WORLD_SPACE) { setPhysicsActor(physicsActor); }
		virtual ~PxeStaticPhysicsRenderObject() { if (_physicsActor) _physicsActor->release(); }
		const physx::PxActor* getPhysicsActor() const { return _physicsActor; }
		physx::PxActor* getPhysicsActor() { return _physicsActor; }

	protected:
		void setPhysicsActor(physx::PxActor* physicsActor) {
			_physicsActor = physicsActor; 
			if (_physicsActor) {
				physicsActor->userData = this;
			}
		}

	private:
		physx::PxActor* _physicsActor;
	};
}

#endif // !PXENGINE_STATIC_PHYSICS_RENDER_OBJECT_H_
