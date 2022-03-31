#ifndef STATICPHYSICSOBJECT_H
#define STATICPHYSICSOBJECT_H

#include "PhysicsBase.h"
#include "PhysicsObject.h"
#include "PhysicsScene.h"
#include "PxPhysicsAPI.h"
#include "RefCounted.h"
#include "glm/vec3.hpp"
#include "util/DataBuffer.h"

namespace px::physics {
class StaticPhysicsObject : public PhysicsObject {
 public:
  StaticPhysicsObject(const glm::vec3& position,
                      util::DataBuffer<float>* vertices,
                      util::DataBuffer<uint32_t>* indices,
                      const float vertexStride = 3, PhysicsScene* scene = nullptr);
  void updateMesh(util::DataBuffer<float>* vertices,
                  util::DataBuffer<uint32_t>* indices,
                  const float vertexStride = 3);
  void setScene(PhysicsScene* scene);
  PhysicsScene* getScene() const;
  virtual ~StaticPhysicsObject();
  physx::PxActor* getPxActor() const;

 private:
  PhysicsScene* _scene;
  physx::PxRigidStatic* _pxStaticBody;
  physx::PxShape* _pxShape;
};
}  // namespace px::physics
#endif  // !STATICPHYSICSOBJECT_H
