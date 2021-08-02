#pragma once
#include "Mesh.h"
#include "reactphysics3d/reactphysics3d.h"

class PhysicsMesh : public virtual Mesh {
 public:
  PhysicsMesh(VertexBufferAttrib* customAttribs[], unsigned short attribCount,
              rp3d::PhysicsWorld* world, rp3d::Transform rbTransform);
  PhysicsMesh(rp3d::PhysicsWorld* world, rp3d::Transform rbTransform);
  virtual ~PhysicsMesh();
  virtual void updateTransfrom(float dt);
 private:
  rp3d::RigidBody* _rb;
  rp3d::PhysicsWorld* _world;
};