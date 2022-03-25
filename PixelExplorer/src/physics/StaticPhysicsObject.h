#ifndef STATICPHYSICSOBJECT_H
#define STATICPHYSICSOBJECT_H
#include "PhysicsScene.fwd.h"
#include "PxPhysicsAPI.h"
#include "RefCounted.h"
#include "StaticPhysicsObject.fwd.h"
#include "DataBuffer.h"
#include "glm/vec3.hpp"

class StaticPhysicsObject : public RefCounted {
  friend class PhysicsScene;

 public:
  void UpdateMesh(DataBuffer<float>* vertices, DataBuffer<uint32_t>* indices,
                  const float vertexStride);
  virtual ~StaticPhysicsObject();

 private:
  PhysicsScene* _scene;
  physx::PxRigidStatic* _pxStaticBody;
  physx::PxShape* _pxShape;

  StaticPhysicsObject(PhysicsScene* scene, const glm::vec3& position, DataBuffer<float>* vertices, DataBuffer<uint32_t>* indices, const float vertexStride);
};
#endif  // !STATICPHYSICSOBJECT_H
