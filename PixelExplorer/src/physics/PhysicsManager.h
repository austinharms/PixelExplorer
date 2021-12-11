#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "PxPhysicsAPI.h"

class PhysicsManager : public physx::PxErrorCallback {
 public:
  static bool Init();
  static void Destroy();
  ~PhysicsManager();

  virtual void reportError(physx::PxErrorCode::Enum code, const char* message,
                           const char* file, int line);

 private:
  static physx::PxDefaultAllocator* s_allocator;
  static PhysicsManager* s_physicsManager;
  static physx::PxFoundation* s_foundation;
  static physx::PxPvd* s_pvd;
  static physx::PxPvdTransport* s_pvdTransport;
  static physx::PxPhysics* s_physics;
  static physx::PxCooking* s_cooking;
  static physx::PxTolerancesScale s_scale;
  static bool s_init;

  PhysicsManager();
};
#endif  // !PHYSICS_MANAGER_H
