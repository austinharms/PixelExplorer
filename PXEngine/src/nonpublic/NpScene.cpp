#include "NpScene.h"

#include "NpLogger.h"
#include "PxeRenderMaterial.h"
#include "PxeRenderObjectInterface.h"
#include "NpEngine.h"

namespace pxengine::nonpublic {
	NpScene::NpScene(physx::PxScene* scene) : _renderCollection(this)
	{
		_physScene = scene;
		_physScene->userData = this;
		_simulationAccumulator = 0;
		_simulationTimestep = 0.01f;
		_simulationScale = 1;
		_userData = nullptr;
		NpEngine::getInstance().grab();
	}

	NpScene::~NpScene()
	{
		_sceneMutex.lock();
		_renderCollection.clear();

		for (PxePhysicsUpdateObjectInterface* phys : _physicsUpdateObjects)
			dynamic_cast<PxeObject*>(phys)->drop();
		_physicsUpdateObjects.clear();

		_physScene->lockWrite();
		for (PxePhysicsObjectInterface* phys : _physicsObjects) {
			if (phys->getPhysicsActor())
				_physScene->removeActor(*(phys->getPhysicsActor()));
			dynamic_cast<PxeObject*>(phys)->drop();
		}

		_physicsObjects.clear();
		_physScene->unlockWrite();

		_physScene->release();
		NpEngine::getInstance().drop();
		_sceneMutex.unlock();
	}

	PXE_NODISCARD const NpRenderCollection& NpScene::getRenderCollection() const
	{
		return _renderCollection;
	}

	PXE_NODISCARD const std::list<PxePhysicsUpdateObjectInterface*>& NpScene::getPhysicsUpdateObjectList() const
	{
		return _physicsUpdateObjects;
	}

	PXE_NODISCARD const std::list<PxePhysicsObjectInterface*>& NpScene::getPhysicsObjectList() const
	{
		return _physicsObjects;
	}

	void NpScene::simulatePhysics(float time)
	{
		std::unique_lock lock(_sceneMutex);
		_simulationAccumulator += time * _simulationScale;
		if (_simulationAccumulator < _simulationTimestep) return;
		_objectMutex.lock_shared();
		_physScene->lockWrite(__FUNCTION__, __LINE__);
		for (PxePhysicsUpdateObjectInterface* phys : _physicsUpdateObjects)
			phys->onPhysics();
		_objectMutex.unlock_shared();
		while (_simulationAccumulator >= _simulationTimestep)
		{
			_simulationAccumulator -= _simulationTimestep;
			_physScene->lockWrite();
			_physScene->simulate(_simulationTimestep);
			_physScene->unlockWrite();
			while (!_physScene->checkResults());
			_physScene->lockWrite();
			_physScene->fetchResults(true);
			_physScene->unlockWrite();
		}

		_physScene->unlockWrite();
	}

	PXE_NODISCARD physx::PxScene* NpScene::getPhysicsScene() const
	{
		std::shared_lock lock(_sceneMutex);
		return _physScene;
	}

	void NpScene::setPhysicsSimulationSpeed(float speed)
	{
		std::unique_lock lock(_sceneMutex);
		_simulationScale = speed;
	}

	PXE_NODISCARD float NpScene::getPhysicsSimulationSpeed() const
	{
		std::shared_lock lock(_sceneMutex);
		return _simulationScale;
	}

	PXE_NODISCARD float NpScene::getPhysicsSimulationAccumulator() const
	{
		std::shared_lock lock(_sceneMutex);
		return _simulationAccumulator;
	}

	void NpScene::setPhysicsSimulationAccumulator(float time)
	{
		std::unique_lock lock(_sceneMutex);
		_simulationAccumulator = time;
	}

	PXE_NODISCARD float NpScene::getPhysicsSimulationStep() const
	{
		std::shared_lock lock(_sceneMutex);
		return _simulationTimestep;
	}

	void NpScene::setPhysicsSimulationStep(float step)
	{
		std::unique_lock lock(_sceneMutex);
		_simulationTimestep = step;
	}

	void NpScene::addObject(PxeObject& obj)
	{
		std::unique_lock lock(_objectMutex);
		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::PHYSICS_OBJECT) {
			PxePhysicsObjectInterface* physObj = dynamic_cast<PxePhysicsObjectInterface*>(&obj);
			if (physObj) {
				physx::PxActor* actor;
				if ((actor = physObj->getPhysicsActor())) {
					if (!actor->getScene()) {
						obj.grab();
						_physScene->lockWrite();
						_physScene->addActor(*actor);
						_physScene->unlockWrite();
						_physicsObjects.emplace_back(physObj);
					}
					else {
						PXE_WARN("Added PxeObject/PxePhysicsObjectInterface with PxActor already assigned to a physics scene");
					}
				}
				else {
					PXE_WARN("Added PxeObject/PxePhysicsObjectInterface with invalid PxActor");
				}
			}
			else {
				PXE_WARN("Added PxeObject with PHYSICS_OBJECT flag that did not inherit from PxePhysicsObjectInterface");
			}
		}

		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::PHYSICS_UPDATE) {
			PxePhysicsUpdateObjectInterface* physObj = dynamic_cast<PxePhysicsUpdateObjectInterface*>(&obj);
			if (physObj) {
				obj.grab();
				_physicsUpdateObjects.emplace_back(physObj);
			}
			else {
				PXE_WARN("Added PxeObject with PHYSICS_UPDATE flag that did not inherit from PxePhysicsUpdateObjectInterface");
			}
		}

		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::RENDER_OBJECT) {
			_renderCollection.addObject(obj);
		}
	}

	void NpScene::removeObject(PxeObject& obj)
	{
		std::unique_lock lock(_objectMutex);
		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::PHYSICS_OBJECT) {
			PxePhysicsObjectInterface* physObj = dynamic_cast<PxePhysicsObjectInterface*>(&obj);
			if (physObj) {
				physx::PxActor* actor;
				if ((actor = physObj->getPhysicsActor())) {
					if (actor->getScene() == _physScene) {
						_physScene->lockWrite();
						_physScene->removeActor(*actor);
						_physScene->unlockWrite();
						if (_physicsObjects.remove(physObj))
							obj.drop();
					}
					else {
						PXE_WARN("Removed PxeObject/PxePhysicsObjectInterface with PxActor not assigned to correct physics scene");
					}
				}
				else {
					PXE_WARN("Removed PxeObject/PxePhysicsObjectInterface with invalid PxActor");
				}
			}
			else {
				PXE_WARN("Removed PxeObject with PHYSICS_OBJECT flag that did not inherit from PxePhysicsObjectInterface");
			}
		}

		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::PHYSICS_UPDATE) {
			PxePhysicsUpdateObjectInterface* physObj = dynamic_cast<PxePhysicsUpdateObjectInterface*>(&obj);
			if (physObj) {
				if (_physicsUpdateObjects.remove(physObj)) {
					obj.drop();
				}
				else {
					PXE_WARN("Removed PxeObject/PxePhysicsUpdateObjectInterface that was not added to the PxeScene");
				}
			}
			else {
				PXE_WARN("Removed PxeObject with PHYSICS_UPDATE flag that did not inherit from PxePhysicsUpdateObjectInterface");
			}
		}

		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::RENDER_OBJECT) {
			_renderCollection.removeObject(obj);
		}
	}

	PXE_NODISCARD void* NpScene::getUserData() const {
		std::shared_lock lock(_sceneMutex);
		return _userData;
	}

	void NpScene::setUserData(void* data) {
		std::unique_lock lock(_sceneMutex);
		_userData = data;
	}

	void NpScene::acquireReadLock()
	{
		_sceneMutex.lock_shared();
	}

	void NpScene::releaseReadLock()
	{
		_sceneMutex.unlock_shared();
	}

	void NpScene::acquireWriteLock()
	{
		_sceneMutex.lock();
	}

	void NpScene::releaseWriteLock()
	{
		_sceneMutex.unlock();
	}

	void NpScene::acquireObjectsReadLock()
	{
		_objectMutex.lock_shared();
	}

	void NpScene::releaseObjectsReadLock()
	{
		_objectMutex.unlock_shared();
	}

	void NpScene::acquireObjectsWriteLock()
	{
		_objectMutex.lock();
	}

	void NpScene::releaseObjectsWriteLock()
	{
		_objectMutex.unlock();
	}
}