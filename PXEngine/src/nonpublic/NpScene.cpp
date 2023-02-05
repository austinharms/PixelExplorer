#include "NpScene.h"

#include "NpLogger.h"
#include "PxeRenderMaterial.h"
#include "NpEngine.h"

namespace pxengine::nonpublic {
	NpScene::NpScene(physx::PxScene* scene)
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
		for (PxeGUIObjectInterface* gui : _guiObjects)
			reinterpret_cast<PxeObject*>(gui)->drop();
		_guiObjects.clear();

		for (PxeGeometryObjectInterface* geom : _geometryObjects)
			reinterpret_cast<PxeObject*>(geom)->drop();
		_geometryObjects.clear();

		for (PxePhysicsUpdateObjectInterface* phys : _physicsUpdateObjects)
			reinterpret_cast<PxeObject*>(phys)->drop();
		_physicsObjects.clear();

		_physScene->lockWrite();
		for (PxePhysicsObjectInterface* phys : _physicsObjects) {
			if (phys->getPhysicsActor())
				_physScene->removeActor(*(phys->getPhysicsActor()));
			reinterpret_cast<PxeObject*>(phys)->drop();
		}

		_physicsObjects.clear();
		_physScene->unlockWrite();

		_physScene->release();
		NpEngine::getInstance().drop();
		_sceneMutex.unlock();
	}

	PXE_NODISCARD const std::list<PxeGUIObjectInterface*>& NpScene::getGUIObjectList() const
	{
		return _guiObjects;
	}

	PXE_NODISCARD const std::list<PxeGeometryObjectInterface*>& NpScene::getGeometryObjectList() const
	{
		return _geometryObjects;
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
		_physScene->lockWrite(__FUNCTION__, __LINE__);
		_objectMutex.lock_shared();
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

		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::GUI_UPDATE) {
			PxeGUIObjectInterface* guiObj = dynamic_cast<PxeGUIObjectInterface*>(&obj);
			if (guiObj) {
				obj.grab();
				_guiObjects.emplace_back(guiObj);
			}
			else {
				PXE_WARN("Added PxeObject with GUI_UPDATE flag that did not inherit from PxeGUIObjectInterface");
			}
		}

		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::GEOMETRY_UPDATE) {
			PxeGeometryObjectInterface* geoObj = dynamic_cast<PxeGeometryObjectInterface*>(&obj);
			if (geoObj) {
				obj.grab();
				if (_geometryObjects.empty()) {
					_geometryObjects.emplace_back(geoObj);
				}
				else {
					void* materialIndex = &(geoObj->getRenderMaterial());
					// TODO Optimize this to also sort by shaders in materials to allow for minimal shader changes when rendering
					for (auto geoItr = _geometryObjects.begin(); geoItr != _geometryObjects.end(); ++geoItr) {
						if (materialIndex <= &(geoObj->getRenderMaterial())) {
							_geometryObjects.emplace(geoItr, geoObj);
							break;
						}
					}
				}
			}
			else {
				PXE_WARN("Added PxeObject with GEOMETRY_UPDATE flag that did not inherit from PxeGeometryObjectInterface");
			}
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

		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::GUI_UPDATE) {
			PxeGUIObjectInterface* guiObj = dynamic_cast<PxeGUIObjectInterface*>(&obj);
			if (guiObj) {
				if (_guiObjects.remove(guiObj)) {
					obj.drop();
				}
				else {
					PXE_WARN("Removed PxeObject/PxeGUIObjectInterface that was not added to the PxeScene");
				}
			}
			else {
				PXE_WARN("Removed PxeObject with GUI_UPDATE flag that did not inherit from PxeGUIObjectInterface");
			}
		}

		if (obj.getObjectFlags() & (PxeObjectFlagsType)PxeObjectFlags::GEOMETRY_UPDATE) {
			PxeGeometryObjectInterface* geoObj = dynamic_cast<PxeGeometryObjectInterface*>(&obj);
			if (geoObj) {
				if (_geometryObjects.remove(geoObj)) {
					obj.drop();
				}
				else {
					PXE_WARN("Removed PxeObject/PxeGeometryObjectInterface that was not added to the PxeScene");
				}
			}
			else {
				PXE_WARN("Removed PxeObject with GEOMETRY_UPDATE flag that did not inherit from PxeGeometryObjectInterface");
			}
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