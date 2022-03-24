#ifndef PHYSICSBASE_H
#define PHYSICSBASE_H

#include "PxPhysicsAPI.h"
#include "RefCounted.h"

class PhysicsBase : public RefCounted, public physx::PxErrorCallback {
 public:
  static PhysicsBase* CreatePhysicsBase();
  static void* DropPhysicsBase();

  void reportError(physx::PxErrorCode::Enum code, const char* message,
                   const char* file, int line);
  virtual ~PhysicsBase();

 private:
  static PhysicsBase* s_physicsBase;

  physx::PxDefaultAllocator* _pxAllocator;
  physx::PxFoundation* _pxFoundation;
  physx::PxPvd* _pxPVD;
  physx::PxPvdTransport* _pxPVDTransport;
  physx::PxPhysics* _pxPhysics;
  physx::PxCooking* _pxCooking;
  physx::PxTolerancesScale _pxScale;
  physx::PxMaterial* _pxDefaultMaterial;

  PhysicsBase();
};
#endif  // !PHYSICSBASE_H
