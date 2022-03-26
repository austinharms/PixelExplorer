#ifndef PHYSICSSCENE_H
#define PHYSICSSCENE_H

#include "PhysicsBase.fwd.h"
#include "PhysicsScene.fwd.h"
#include "StaticPhysicsObject.fwd.h"
#include "PxPhysicsAPI.h"
#include "RefCounted.h"
#include "DataBuffer.h"
#include "glm/vec3.hpp"
namespace px::physics {
class PhysicsScene : public RefCounted {
  friend class PhysicsBase;
  friend class StaticPhysicsObject;

 public:
  StaticPhysicsObject* createStaticObject(const glm::vec3& position,
                                          DataBuffer<float>* vertices,
                                          DataBuffer<uint32_t>* indices,
                                          const float vertexStride = 3);
  virtual ~PhysicsScene();

 private:
  physx::PxScene* _pxScene;
  PhysicsBase* _base;

  PhysicsScene(PhysicsBase* base);
};
}  // namespace px::physics
#endif