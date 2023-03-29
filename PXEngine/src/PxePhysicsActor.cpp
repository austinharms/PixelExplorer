#include "PxePhysicsActor.h"

#include "PxeObject.h"
#include "PxeScene.h"
#include "PxRigidActor.h"
#include "PxePhysicsShape.h"
#include "PxeLogger.h"

namespace pxengine {
	PxePhysicsActor::PxePhysicsActor()
	{
	}

	PxePhysicsActor::~PxePhysicsActor()
	{
	}

	PXE_NODISCARD bool PxePhysicsActor::checkComponentRequirements(pxengine::PxeObject& object)
	{
		if (object.hasComponent<PxePhysicsActor>()) {
			PXE_WARN("Only one PxePhysicsActor can be attached to a PxeObject");
			return false;
		}

		return PxePhysicsComponent::checkComponentRequirements(object);
	}

	void PxePhysicsActor::addToObject(pxengine::PxeObject& object)
	{
		PxeComponent** pxeShapes = object.getComponents<PxePhysicsShape>();
		if (pxeShapes) {
			physx::PxRigidActor& actor = getActor();
			if (!actor.getScene()) {
				PxeSize idx = 0;
				while (pxeShapes[idx])
				{
					actor.attachShape(static_cast<PxePhysicsShape*>(pxeShapes[idx])->getShape());
					++idx;
				}
			}
			else {
				PXE_ERROR("Failed to add existing PxePhysicsShapes to PxePhysicsActor, actor is part of scene");
			}

			free(pxeShapes);
		}
		else {
			PXE_ERROR("Failed to get existing PxePhysicsShapes");
		}

		PxePhysicsComponent::addToObject(object);
	}

	void PxePhysicsActor::removeFromObject(pxengine::PxeObject& object)
  	{
		physx::PxRigidActor& actor = getActor();
		if (!actor.getScene()) {
			uint32_t shapeCount = actor.getNbShapes();
			if (shapeCount) {
				physx::PxShape** shapes = static_cast<physx::PxShape**>(alloca(shapeCount * sizeof(physx::PxShape*)));
				shapeCount = actor.getShapes(shapes, shapeCount);
				for (uint32_t i = 0; i < shapeCount; ++i)
					actor.detachShape(*shapes[i]);
			}
		}
		else {
			PXE_ERROR("Failed to remove PxePhysicsShapes from PxePhysicsActor, actor is part of scene");
		}

		PxePhysicsComponent::removeFromObject(object);
	}

	void PxePhysicsActor::addToScene(pxengine::PxeScene& scene)
	{
		PxePhysicsComponent::addToScene(scene);
	}

	void PxePhysicsActor::removeFromScene(pxengine::PxeScene& scene)
	{
		PxePhysicsComponent::removeFromScene(scene);
	}

	void PxePhysicsActor::addShape(PxePhysicsShape& shape)
	{
		physx::PxRigidActor& actor = getActor();
		physx::PxScene* scene = actor.getScene();
		if (scene) scene->lockWrite(__FILE__, __LINE__);
		if (!actor.attachShape(shape.getShape())) {
			PXE_ERROR("Failed to attachShape to PxePhysicsActor");
		}

		if (scene) scene->unlockWrite();
	}

	void PxePhysicsActor::removeShape(PxePhysicsShape& shape)
	{
		physx::PxRigidActor& actor = getActor();
		physx::PxScene* scene = actor.getScene();
		if (scene) scene->lockWrite(__FILE__, __LINE__);
		actor.detachShape(shape.getShape());
		if (scene) scene->unlockWrite();
	}

}
