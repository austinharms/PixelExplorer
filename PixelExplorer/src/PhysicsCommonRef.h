#pragma once
#include "PhysicsWorldRef.h"
#include "RefCounted.h"
#include "reactphysics3d/reactphysics3d.h"

class PhysicsCommonRef : public virtual RefCounted, public rp3d::PhysicsCommon {
 public:
  inline PhysicsCommonRef() {}
  inline virtual ~PhysicsCommonRef() {}
  inline PhysicsWorldRef* createPhysicsWorldRef(
      const rp3d::PhysicsWorld::WorldSettings& worldSettings) {
    rp3d::Profiler* profiler = nullptr;

#ifdef IS_RP3D_PROFILING_ENABLED

    profiler = createProfiler();

    // Add a destination file for the profiling data
    profiler->addFileDestination(
        "rp3d_profiling_" + worldSettings.worldName + ".txt",
        rp3d::Profiler::Format::Text);

#endif

    PhysicsWorldRef* world = new (mMemoryManager.allocate(
        rp3d::MemoryManager::AllocationType::Heap, sizeof(PhysicsWorldRef)))
        PhysicsWorldRef(this, mMemoryManager, worldSettings, profiler);

    mPhysicsWorlds.add(world);

    return world;
  }

  inline void destroyPhysicsWorldRef(PhysicsWorldRef* world) {
    // Release allocated memory
    mMemoryManager.release(rp3d::MemoryManager::AllocationType::Heap, world,
                           sizeof(PhysicsWorldRef));

    mPhysicsWorlds.remove(world);
    drop();
  }
};