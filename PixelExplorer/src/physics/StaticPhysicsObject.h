#ifndef STATICPHYSICSOBJECT_H
#define STATICPHYSICSOBJECT_H
#include "PhysicsScene.fwd.h"
#include "PxPhysicsAPI.h"
#include "RefCounted.h"
#include "StaticPhysicsObject.fwd.h"
#include "util/DataBuffer.h"
#include "glm/vec3.hpp"

namespace px::physics {
class StaticPhysicsObject : public RefCounted {
  friend class PhysicsScene;

 public:
  void updateMesh(util::DataBuffer<float>* vertices,
                  util::DataBuffer<uint32_t>* indices,
                  const float vertexStride);
  virtual ~StaticPhysicsObject();

 private:
  PhysicsScene* _scene;
  physx::PxRigidStatic* _pxStaticBody;
  physx::PxShape* _pxShape;

  StaticPhysicsObject(PhysicsScene* scene, const glm::vec3& position,
                      util::DataBuffer<float>* vertices,
                      util::DataBuffer<uint32_t>* indices,
                      const float vertexStride);
};
}  // namespace px::physics
#endif  // !STATICPHYSICSOBJECT_H
