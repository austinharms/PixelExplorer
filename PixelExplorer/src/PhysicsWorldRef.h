#pragma once
#include "PhysicsCommonRef.h"
#include "RefCounted.h"
#include "reactphysics3d/reactphysics3d.h"

class PhysicsWorldRef : public virtual RefCounted,
                        public virtual rp3d::PhysicsWorld {
 public:
  inline PhysicsWorldRef(PhysicsCommonRef* physicsCom,
                         rp3d::MemoryManager& memoryManager,
                         const WorldSettings& worldSettings,
                         rp3d::Profiler* profiler)
      : RefCounted(),
        rp3d::PhysicsWorld(memoryManager, worldSettings, profiler),
        _physicsCommon(physicsCom) {
    _physicsCommon->grab();
  }

  inline virtual ~PhysicsWorldRef() {
    _physicsCommon->destroyPhysicsWorldRef(this);
  }

 private:
  PhysicsCommonRef* _physicsCommon;
};