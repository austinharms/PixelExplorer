#ifndef PHYSICSBASE_H
#define PHYSICSBASE_H

#include "PhysicsBase.fwd.h"
#include "PhysicsScene.fwd.h"
#include "PxPhysicsAPI.h"
#include "RefCounted.h"

class PhysicsBase : public RefCounted, public physx::PxErrorCallback {
  friend class PhysicsScene;
 public:
  static PhysicsBase* createPhysicsBase();
  static void dropPhysicsBase();

  physx::PxTriangleMesh* bakePxMesh(physx::PxTriangleMeshDesc& desc);
  physx::PxPhysics* getPxPhysics() const;
  physx::PxMaterial* getDefaultPxMaterial() const;
  PhysicsScene* createPhysicsScene();
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
  physx::PxCpuDispatcher* _pxDispatcher;
  physx::PxSimulationFilterShader _pxDefaultFilter;

  PhysicsBase();
};
#endif  // !PHYSICSBASE_H
