#ifndef PHYSICSSCENE_H
#define PHYSICSSCENE_H

#include "PhysicsObject.h"
#include "PhysicsBase.h"
#include "PxPhysicsAPI.h"
#include "RefCounted.h"
namespace px::physics {
class PhysicsScene : public RefCounted {
 public:
  virtual ~PhysicsScene();
  void insertObject(PhysicsObject* obj);
  void removeObject(PhysicsObject* obj);

 private:
  physx::PxScene* _pxScene;

  PhysicsScene();
};
}  // namespace px::physics
#endif