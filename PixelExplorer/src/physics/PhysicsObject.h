#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "PxPhysicsAPI.h"
#include "RefCounted.h"

namespace px::physics {
class PhysicsObject : public RefCounted {
 public:
  inline virtual ~PhysicsObject() {}
  virtual physx::PxActor* getPxActor() const = 0;

 protected:
  inline PhysicsObject() {}
};
}  // namespace px::physics
#endif  // !PHYSICSOBJECT_H