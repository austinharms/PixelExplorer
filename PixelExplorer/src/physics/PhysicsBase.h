#ifndef PHYSICSBASE_H
#define PHYSICSBASE_H

#include "PxPhysicsAPI.h"

namespace px::physics {
class PhysicsBase : public physx::PxErrorCallback {
 public:
  static PhysicsBase& getInstance();

  physx::PxPhysics* getPxPhysics() const;
  physx::PxMaterial* getDefaultPxMaterial() const;
  physx::PxCooking* getPxCooking() const;
  physx::PxCpuDispatcher* getPxDispatcher() const;
  physx::PxSimulationFilterShader getPxSimulationFilter() const;
  physx::PxTolerancesScale* getPxScale() const;
  void reportError(physx::PxErrorCode::Enum code, const char* message,
                   const char* file, int line);
  ~PhysicsBase();
  PhysicsBase(PhysicsBase const&) = delete;
  void operator=(PhysicsBase const&) = delete;

 private:
  physx::PxDefaultAllocator* _pxAllocator;
  physx::PxFoundation* _pxFoundation;
  physx::PxPvd* _pxPVD;
  physx::PxPvdTransport* _pxPVDTransport;
  physx::PxPhysics* _pxPhysics;
  physx::PxCooking* _pxCooking;
  physx::PxTolerancesScale* _pxScale;
  physx::PxMaterial* _pxDefaultMaterial;
  physx::PxCpuDispatcher* _pxDispatcher;
  physx::PxSimulationFilterShader _pxDefaultFilter;

  PhysicsBase();
};
}  // namespace px::physics
#endif  // !PHYSICSBASE_H
