#include "PxeStaticPhysicsActor.h"

#include <new>

#include "PxeEngine.h"
#include "PxScene.h"
#include "foundation/PxMat44.h"
#include "glm/gtc/type_ptr.hpp"

namespace pxengine {
	PxeStaticPhysicsActor* PxeStaticPhysicsActor::create()
	{
		PxeEngine& engine = PxeEngine::getInstance();
		physx::PxTransform t(physx::PxIdentity);
		physx::PxRigidStatic* actor = engine.getPhysicsBase().createRigidStatic(t);
		if (!actor) {
			//PXE_ERROR("Failed to create PxeStaticPhysicsActor's physx::PxRigidStatic actor");
			return nullptr;
		}

		PxeStaticPhysicsActor* pxeActor = new(std::nothrow) PxeStaticPhysicsActor(*actor);
		if (!pxeActor) {
			//PXE_ERROR("Failed to allocate PxeStaticPhysicsActor");
			actor->release();
			return nullptr;
		}

		return pxeActor;
	}

	PxeStaticPhysicsActor::PxeStaticPhysicsActor(physx::PxRigidStatic& actor) : _actor(actor)
	{
		_lastTransform = glm::mat4(1.0f);
		//glm::make_mat4(physx::PxMat44(getPhysicsRigidActor()->getGlobalPose()).front())
	}

	PxeStaticPhysicsActor::~PxeStaticPhysicsActor()
	{
		_actor.release();
	}

	PXE_NODISCARD bool PxeStaticPhysicsActor::checkComponentRequirements(pxengine::PxeObject& object)
	{
		return PxePhysicsActor::checkComponentRequirements(object);
	}

	void PxeStaticPhysicsActor::addToObject(pxengine::PxeObject& object)
	{
		PxePhysicsActor::addToObject(object);
	}

	void PxeStaticPhysicsActor::removeFromObject(pxengine::PxeObject& object)
	{
		PxePhysicsActor::removeFromObject(object);
	}

	void PxeStaticPhysicsActor::addToScene(pxengine::PxeScene& scene)
	{
		physx::PxScene& pxScene = scene.getPhysicsScene();
		pxScene.lockWrite(__FILE__, __LINE__);
		pxScene.addActor(_actor);
		pxScene.unlockWrite();
		PxePhysicsActor::addToScene(scene);
	}

	void PxeStaticPhysicsActor::removeFromScene(pxengine::PxeScene& scene)
	{
		physx::PxScene& pxScene = scene.getPhysicsScene();
		pxScene.lockWrite(__FILE__, __LINE__);
		pxScene.removeActor(_actor);
		pxScene.unlockWrite();
		PxePhysicsActor::removeFromScene(scene);
	}

	void PxeStaticPhysicsActor::onPhysics()
	{
		if (_lastTransform != getParentObject()->getTransform()) {
			_lastTransform = getParentObject()->getTransform();
			physx::PxScene* scene = _actor.getScene();
			scene->lockWrite(__FILE__, __LINE__);
			_actor.setGlobalPose(physx::PxTransform(physx::PxMat44(glm::value_ptr(_lastTransform))));
			scene->unlockWrite();
		}
	}

	physx::PxRigidActor& PxeStaticPhysicsActor::getActor()
	{
		return _actor;
	}
}
