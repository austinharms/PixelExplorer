#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <assert.h>

#include "PxPhysicsAPI.h"

class PhysicsManager : public physx::PxErrorCallback {
 public:
  static bool Init();
  static void Destroy();
  ~PhysicsManager();

  virtual void reportError(physx::PxErrorCode::Enum code, const char* message,
                           const char* file, int line);
  static physx::PxTolerancesScale* GetScale() { return &s_scale; }
  static physx::PxCooking* GetCooking() { return s_cooking; }
  static physx::PxScene* CreateScene();
  static bool GetInitialized() { return s_init; }
  static physx::PxTriangleMesh* CreatePxMesh(physx::PxTriangleMeshDesc& desc) {
    if (!s_init) return nullptr;
    return s_cooking->createTriangleMesh(
        desc, s_physics->getPhysicsInsertionCallback());
  }

  static physx::PxRigidStatic* CreatePxStaticActor(
      const physx::PxTransform& t, physx::PxTriangleMeshGeometry& geom) {
    return CreatePxStaticActor(t, geom, *s_defaultMaterial);
  }

    static physx::PxRigidStatic* CreatePxStaticActor(
      const physx::PxTransform& t, physx::PxTriangleMeshGeometry& geom, const physx::PxMaterial& material) {
    physx::PxShape* shape =
        s_physics->createShape(geom, material, true);
    shape->setName("Chunk");
    physx::PxRigidStatic* body = s_physics->createRigidStatic(t);
    body->attachShape(*shape);
    return body;
  }

 private:
  static physx::PxDefaultAllocator* s_allocator;
  static PhysicsManager* s_physicsManager;
  static physx::PxFoundation* s_foundation;
  static physx::PxPvd* s_pvd;
  static physx::PxPvdTransport* s_pvdTransport;
  static physx::PxPhysics* s_physics;
  static physx::PxCooking* s_cooking;
  static physx::PxTolerancesScale s_scale;
  static physx::PxMaterial* s_defaultMaterial;
  static bool s_init;

  PhysicsManager();
};
#endif  // !PHYSICS_MANAGER_H
