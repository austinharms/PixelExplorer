#include "PxeDynamicPhysicsActor.h"

#include <new>

#include "PxeEngine.h"
#include "PxScene.h"
#include "foundation/PxMat44.h"
#include "glm/gtc/type_ptr.hpp"

namespace pxengine {
	PxeDynamicPhysicsActor* PxeDynamicPhysicsActor::create()
	{
		PxeEngine& engine = PxeEngine::getInstance();
		physx::PxTransform t(physx::PxIdentity);
		physx::PxRigidDynamic* actor = engine.getPhysicsBase().createRigidDynamic(t);
		if (!actor) {
			PXE_ERROR("Failed to create PxeDynamicPhysicsActor's physx::PxRigidStatic actor");
			return nullptr;
		}

		PxeDynamicPhysicsActor* pxeActor = new(std::nothrow) PxeDynamicPhysicsActor(*actor);
		if (!pxeActor) {
			PXE_ERROR("Failed to allocate PxeDynamicPhysicsActor");
			actor->release();
			return nullptr;
		}

		return pxeActor;
	}

	PxeDynamicPhysicsActor::PxeDynamicPhysicsActor(physx::PxRigidDynamic& actor) : _actor(actor)
	{
		_lastTransform = glm::mat4(1.0f);
	}

	PxeDynamicPhysicsActor::~PxeDynamicPhysicsActor()
	{
		_actor.release();
	}

	PXE_NODISCARD bool PxeDynamicPhysicsActor::checkComponentRequirements(pxengine::PxeObject& object)
	{
		return PxePhysicsActor::checkComponentRequirements(object);
	}

	void PxeDynamicPhysicsActor::addToObject(pxengine::PxeObject& object)
	{
		PxePhysicsActor::addToObject(object);
	}

	void PxeDynamicPhysicsActor::removeFromObject(pxengine::PxeObject& object)
	{
		PxePhysicsActor::removeFromObject(object);
	}

	void PxeDynamicPhysicsActor::addToScene(pxengine::PxeScene& scene)
	{
		physx::PxScene& pxScene = scene.getPhysicsScene();
		pxScene.lockWrite(__FILE__, __LINE__);
		pxScene.addActor(_actor);
		pxScene.unlockWrite();
		PxePhysicsActor::addToScene(scene);
	}

	void PxeDynamicPhysicsActor::removeFromScene(pxengine::PxeScene& scene)
	{
		physx::PxScene& pxScene = scene.getPhysicsScene();
		pxScene.lockWrite(__FILE__, __LINE__);
		pxScene.removeActor(_actor);
		pxScene.unlockWrite();
		PxePhysicsActor::removeFromScene(scene);
	}

	void PxeDynamicPhysicsActor::onPhysics()
	{
		physx::PxScene* scene = _actor.getScene();
		if (_lastTransform != getParentObject()->getTransform()) {
			_lastTransform = getParentObject()->getTransform();
			scene->lockWrite(__FILE__, __LINE__);
			_actor.setGlobalPose(physx::PxTransform(physx::PxMat44(glm::value_ptr(_lastTransform))));
			scene->unlockWrite();
		}
		else {
			scene->lockRead(__FILE__, __LINE__);
			_lastTransform = glm::make_mat4(physx::PxMat44(_actor.getGlobalPose()).front());
			scene->unlockRead();
			getParentObject()->setTransform(_lastTransform);
		}
	}

	physx::PxRigidActor& PxeDynamicPhysicsActor::getActor()
	{
		return _actor;
	}
}
