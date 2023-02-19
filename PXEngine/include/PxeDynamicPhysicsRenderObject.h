#ifndef PXENGINE_DYNAMIC_PHYSICS_RENDER_OBJECT_H_
#define PXENGINE_DYNAMIC_PHYSICS_RENDER_OBJECT_H_
#include "PxeTypes.h"
#include "PxeStaticPhysicsRenderObject.h"
#include "PxePhysicsUpdateObjectInterface.h"
#include "PxeRenderMaterialInterface.h"

namespace pxengine {
	// Base class for rendering object with a physx PxRigidActor that changes position in world space
	// Note: this object may only be in one scene at once due to physx limitations
	// Note: this assumes it has the only reference to the physx PxRigidActor and calls release on destruction
	class PxeDynamicPhysicsRenderObject : public PxeStaticPhysicsRenderObject, public PxePhysicsUpdateObjectInterface
	{
	public:
		static const constexpr PxeObjectFlags DEFAULT_OBJECT_FLAGS = (PxeObjectFlags)((PxeObjectFlagsType)PxeObjectFlags::RENDER_OBJECT | (PxeObjectFlagsType)PxeObjectFlags::PHYSICS_OBJECT | (PxeObjectFlagsType)PxeObjectFlags::PHYSICS_UPDATE);

		PxeDynamicPhysicsRenderObject(PxeRenderMaterialInterface& material, physx::PxRigidActor* physicsActor = nullptr, PxeObjectFlags flags = DEFAULT_OBJECT_FLAGS) : 
			PxeStaticPhysicsRenderObject(material, physicsActor, flags) {}
		virtual ~PxeDynamicPhysicsRenderObject() = default;


		// ##### PxeRenderObjectInterface API #####

		// This method should draw the object to the current framebuffer
		// Note: you can assume a valid OpenGl context
		// Note: this requires the RENDER_OBJECT flag to be set (flag is set by default)
		virtual void onRender() override = 0;

		
		// ##### PxePhysicsUpdateObjectInterface API #####

		// This method is called before every scene physics update
		// Note: the physx actor is guaranteed to be read/writable during this call
		// Note: this requires the PHYSICS_UPDATE flag to be set on the base PxeObject (flag set by default)
		virtual void onPhysics() {
			if (getPhysicsRigidActor())
				positionMatrix = glm::make_mat4(physx::PxMat44(getPhysicsRigidActor()->getGlobalPose()).front());
		};
	};
}

#endif // !PXENGINE_STATIC_PHYSICS_RENDER_OBJECT_H_
