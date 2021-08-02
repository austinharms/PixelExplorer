#pragma once
#include "PhysicsCommonRef.fwd.h"
#include "PhysicsWorldRef.fwd.h"
#include "RefCounted.h"
#include "reactphysics3d/reactphysics3d.h"

class PhysicsWorldRef : public virtual RefCounted {
 public:
  PhysicsWorldRef(PhysicsCommonRef* com, rp3d::PhysicsWorld* world);

  virtual ~PhysicsWorldRef();

  rp3d::PhysicsWorld* getPhysicsWorld() const { return _physicsWorld; }

  explicit operator rp3d::PhysicsWorld*() const { return _physicsWorld; }

 private:
  PhysicsCommonRef* _physicsCommon;
  rp3d::PhysicsWorld* _physicsWorld;
};

#include "PhysicsCommonRef.h"
inline PhysicsWorldRef::PhysicsWorldRef(PhysicsCommonRef* com,
                                        rp3d::PhysicsWorld* world) {
  _physicsCommon = com;
  _physicsCommon->grab();
  _physicsWorld = world;
}

inline PhysicsWorldRef::~PhysicsWorldRef() {
  _physicsCommon->destroyPhysicsWorld(_physicsWorld);
  _physicsCommon->drop();
}