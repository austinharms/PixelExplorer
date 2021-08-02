#pragma once
#include "PhysicsCommonRef.fwd.h"
#include "PhysicsWorldRef.fwd.h"
#include "RefCounted.h"
#include "reactphysics3d/reactphysics3d.h"

class PhysicsCommonRef : public virtual RefCounted,
                         public virtual rp3d::PhysicsCommon {
 public:
  PhysicsCommonRef() : RefCounted() {}
  virtual ~PhysicsCommonRef() {}

  PhysicsWorldRef* createPhysicsWorldRef(
      const rp3d::PhysicsWorld::WorldSettings& worldSettings);

 private:
};

#include "PhysicsWorldRef.h"
inline PhysicsWorldRef* PhysicsCommonRef::createPhysicsWorldRef(
    const rp3d::PhysicsWorld::WorldSettings& worldSettings) {
  return new PhysicsWorldRef(this, createPhysicsWorld(worldSettings));
}