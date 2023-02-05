#ifndef PXENGINE_SCENE_H_
#define PXENGINE_SCENE_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeObject.h"
#include "PxScene.h"

namespace pxengine {
	// Collection of PxeObjects and wrapper for physx::PxScene
	class PxeScene : public PxeRefCount
	{
	public:
		PxeScene() = default;
		virtual ~PxeScene() = default;

		// Returns the internal physx PxScene
		// Note: the physx PxScene is simulated/updated every frame
		// Note: delta time is added to the physics simulation accumulator see setPhysicsSimulationSpeed to change this behavior
		virtual PXE_NODISCARD physx::PxScene* getPhysicsScene() const = 0;

		// Sets the physics simulation step in seconds (unscaled/real time)
		// Note: this should only be changed before the first rendered frame for best simulation stability
		virtual void setPhysicsSimulationStep(float step) = 0;

		// Sets the physics time scale to allow for speeding up or slowing down the physics simulation
		// Note: {speed} greater or equal to 0 (speed >= 0)
		// Note: set to 0 to disable automatically adding delta time to the physics simulation accumulator
		virtual void setPhysicsSimulationSpeed(float speed) = 0;

		// Returns the physics time scale
		virtual PXE_NODISCARD float getPhysicsSimulationSpeed() const = 0;

		// Returns the amount of time in seconds (unscaled/real time) in the physics simulation accumulator
		virtual PXE_NODISCARD float getPhysicsSimulationAccumulator() const = 0;

		// Sets the amount of time in seconds (unscaled/real time) in the physics simulation accumulator
		virtual void setPhysicsSimulationAccumulator(float time) = 0;

		// Returns the physics simulation step in seconds (unscaled/real time)
		virtual PXE_NODISCARD float getPhysicsSimulationStep() const = 0;

		// Add PxeObject object to the scene
		virtual void addObject(PxeObject& obj) = 0;

		// Remove PxeObject object from the scene
		virtual void removeObject(PxeObject& obj) = 0;

		// Returns the user data void*
		// Note: this is purely for application uses and is not used by the engine 
		virtual PXE_NODISCARD void* getUserData() const = 0;

		// Sets the user data void*
		// Note: this is purely for application uses and is not used by the engine 
		virtual void setUserData(void* data) = 0;

		PxeScene(const PxeScene& other) = delete;
		PxeScene operator=(const PxeScene& other) = delete;
	};
}
#endif // !PXENGINE_SCENE_H_
