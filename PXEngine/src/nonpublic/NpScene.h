#ifndef PXENGINE_NONPUBLIC_SCENE_H_
#define PXENGINE_NONPUBLIC_SCENE_H_
#include <list>
#include <mutex>
#include <shared_mutex>

#include "PxeTypes.h"
#include "PxeScene.h"
#include "PxeObject.h"
#include "PxePhysicsObjectInterface.h"
#include "PxePhysicsUpdateObjectInterface.h"
#include "PxeGeometryObjectInterface.h"
#include "PxeGUIObjectInterface.h"
#include "PxScene.h"

namespace pxengine::nonpublic {
	class NpScene : public PxeScene
	{
	public:
		//############# PxeScene API ##################

		PXE_NODISCARD physx::PxScene* getPhysicsScene() const override;
		void setPhysicsSimulationSpeed(float speed) override;
		PXE_NODISCARD float getPhysicsSimulationSpeed() const override;
		PXE_NODISCARD float getPhysicsSimulationAccumulator() const override;
		void setPhysicsSimulationAccumulator(float time) override;
		PXE_NODISCARD float getPhysicsSimulationStep() const override;
		void setPhysicsSimulationStep(float step) override;
		void addObject(PxeObject& obj) override;
		void removeObject(PxeObject& obj) override;
		PXE_NODISCARD void* getUserData() const override;
		void setUserData(void* data) override;


		//############# PRIVATE API ##################
	
		NpScene(physx::PxScene* scene);
		virtual ~NpScene();
		// Note: this does not lock the NpScene
		PXE_NODISCARD const std::list<PxeGUIObjectInterface*>& getGUIObjectList() const;
		// Note: this does not lock the NpScene
		PXE_NODISCARD const std::list<PxeGeometryObjectInterface*>& getGeometryObjectList() const;
		// Note: this does not lock the NpScene
		PXE_NODISCARD const std::list<PxePhysicsUpdateObjectInterface*>& getPhysicsUpdateObjectList() const;
		// Note: this does not lock the NpScene
		PXE_NODISCARD const std::list<PxePhysicsObjectInterface*>& getPhysicsObjectList() const;
		void simulatePhysics(float time);
		void acquireReadLock();
		void releaseReadLock();
		void acquireWriteLock();
		void releaseWriteLock();
		void acquireObjectsReadLock();
		void releaseObjectsReadLock();
		void acquireObjectsWriteLock();
		void releaseObjectsWriteLock();

	private:
		mutable std::shared_mutex _sceneMutex;
		std::shared_mutex _objectMutex;
		void* _userData;
		physx::PxScene* _physScene;
		std::list<PxeGUIObjectInterface*> _guiObjects;
		std::list<PxeGeometryObjectInterface*> _geometryObjects;
		std::list<PxePhysicsUpdateObjectInterface*> _physicsUpdateObjects;
		std::list<PxePhysicsObjectInterface*> _physicsObjects;
		float _simulationTimestep;
		float _simulationAccumulator;
		float _simulationScale;
	};
}
#endif // !PXENGINE_SCENE_H_
