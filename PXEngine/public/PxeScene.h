#ifndef PXENGINE_SCENE_H_
#define PXENGINE_SCENE_H_
PXE_PRIVATE_IMPLEMENTATION_START
#include <list>
PXE_PRIVATE_IMPLEMENTATION_END

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeObject.h"
#include "PxScene.h"

namespace pxengine {
	class PxeEngine;
	class PxeRenderComponent;
	class PxePhysicsComponent;
	class PxeUpdateComponent;

	// Collection of PxeObjects and wrapper for physx::PxScene
	class PxeScene : public PxeRefCount
	{
	public:
		// unused member for application use
		// Note: default value is nullptr
		void* userData;

		// Returns the associated phys::PxScene
		// Note: if PHYSICS_UPDATE flag is set the physx::PxScene will be simulated base on the amount of time in the physics accumulator
		PXE_NODISCARD physx::PxScene& getPhysicsScene() const;

		// Sets how much time (seconds) is simulated each physics step
		// Note: base on how much time is in the physics accumulator the scene may be simulated multiple times an update
		void setPhysicsStepSize(float step);

		// Sets the multiplier used when adding delta time to the physics accumulator
		// Note: this will NOT affect the physics accumulator or physics step size
		// Note: speed values less then 0 will be ignored
		void setPhysicsTimeScale(float speed);

		// Returns the physics time scale
		PXE_NODISCARD float getPhysicsTimeScale() const;

		// Returns the amount of time (seconds) in the physics accumulator
		PXE_NODISCARD float getPhysicsAccumulator() const;

		// Sets the amount of time (seconds) in the physics accumulator
		void setPhysicsSimulationAccumulator(float time);

		// Returns the physics simulation step size (seconds)
		PXE_NODISCARD float getPhysicsStepSize() const;

		// Add PxeObject object to the scene
		bool addObject(PxeObject& obj);

		// Remove PxeObject object from the scene
		void removeObject(PxeObject& obj);

		// Replaces the current scene flags with flags
		void setFlags(PxeSceneFlags flags);

		// Returns the current scene update flags
		PXE_NODISCARD PxeSceneFlags getFlags() const;

		// Sets a scene flag(s)
		void setFlag(PxeSceneFlags flag);

		// Unset's a scene flag(s)
		void clearFlag(PxeSceneFlags flag);

		// Returns if the flag(s) are set
		PXE_NODISCARD bool getFlag(PxeSceneFlags flag) const;

		void onDelete() override;

		virtual ~PxeScene();
		PXE_NOCOPY(PxeScene);

	private:
		PxeScene(physx::PxScene& physicsScene);

		static constexpr PxeSize STORAGE_SIZE = 1528;
		static constexpr PxeSize STORAGE_ALIGN = 8;
		std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
		PXE_PRIVATE_IMPLEMENTATION_START
	public:
		void updatePhysics(float deltaTime);
		void updateComponents();
		void addComponent(PxeComponent& component);
		void removeComponent(PxeComponent& component);
		// acquires components read lock and returns a list of all PxeRenderComponent
		// Note: you must call releaseRenderComponents after to unlock the components read lock
		PXE_NODISCARD const std::list<PxeRenderComponent*>& acquireRenderComponents();
		void releaseRenderComponents();

	private:
		friend class PxeEngine;
		struct Impl;
			
		inline Impl& impl() { return reinterpret_cast<Impl&>(_storage); }
		inline const Impl& impl() const { return reinterpret_cast<const Impl&>(_storage); }
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_SCENE_H_
