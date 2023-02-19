#ifndef PXENGINE_STATIC_PHYSICS_RENDER_OBJECT_H_
#define PXENGINE_STATIC_PHYSICS_RENDER_OBJECT_H_
#include "PxeTypes.h"
#include "PxRigidActor.h"
#include "foundation/PxMat44.h"
#include "PxeObject.h"
#include "PxeRenderObject.h"
#include "PxePhysicsObjectInterface.h"
#include "PxeRenderMaterialInterface.h"

namespace pxengine {
	// Base class for rendering object with a physx PxActor in world space
	// Note: this object may only be in one scene at once due to physx limitations
	// Note: this assumes it has the only reference to the physx PxActor and calls release on destruction
	class PxeStaticPhysicsRenderObject : public PxeObject, public PxeRenderObjectInterface, public PxePhysicsObjectInterface
	{
	public:
		static const constexpr PxeObjectFlags DEFAULT_OBJECT_FLAGS = (PxeObjectFlags)((PxeObjectFlagsType)PxeObjectFlags::RENDER_OBJECT | (PxeObjectFlagsType)PxeObjectFlags::PHYSICS_OBJECT);
		
		PxeStaticPhysicsRenderObject(PxeRenderMaterialInterface& material, physx::PxRigidActor* physicsActor = nullptr, PxeObjectFlags flags = DEFAULT_OBJECT_FLAGS) :
			PxeObject(flags), PxeRenderObjectInterface(material) {
			_physicsActor = nullptr;
			setPhysicsActor(physicsActor);
		}

		virtual ~PxeStaticPhysicsRenderObject() { if (_physicsActor) _physicsActor->release(); }

		PXE_NODISCARD physx::PxRigidActor* getPhysicsRigidActor() const { return _physicsActor; }


		// ##### PxeRenderObjectInterface API #####
		
		PXE_NODISCARD const glm::mat4& getPositionMatrix() const override { return positionMatrix; }

		// This method should draw the object to the current framebuffer
		// Note: you can assume a valid OpenGl context
		// Note: this requires the RENDER_OBJECT flag to be set (flag is set by default)
		virtual void onRender() override = 0;


		// ##### PxePhysicsObjectInterface API #####

		PXE_NODISCARD physx::PxActor* getPhysicsActor() const override { return _physicsActor; }

	protected:
		glm::mat4 positionMatrix;

		// Note: this also copies the physicsActor transform to positionMatrix
		// Note: this does NOT call release on the old actor
		void setPhysicsActor(physx::PxRigidActor* physicsActor) {
			if (_physicsActor) _physicsActor->userData = nullptr;
			if ((_physicsActor = physicsActor)) {
				positionMatrix = glm::make_mat4(physx::PxMat44(_physicsActor->getGlobalPose()).front());
				physicsActor->userData = this;
			}
		}

	private:
		physx::PxRigidActor* _physicsActor;
	};
}

#endif // !PXENGINE_STATIC_PHYSICS_RENDER_OBJECT_H_
