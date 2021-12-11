#include "PhysicsManager.h"

#include <iostream>
using namespace physx;
PxDefaultAllocator* PhysicsManager::s_allocator = nullptr;
PhysicsManager* PhysicsManager::s_physicsManager = nullptr;
PxFoundation* PhysicsManager::s_foundation = nullptr;
PxPvd* PhysicsManager::s_pvd = nullptr;
PxPvdTransport* PhysicsManager::s_pvdTransport = nullptr;
PxPhysics* PhysicsManager::s_physics = nullptr;
PxCooking* PhysicsManager::s_cooking = nullptr;
PxTolerancesScale PhysicsManager::s_scale;
bool PhysicsManager::s_init = false;

bool PhysicsManager::Init() {
  if (!s_init) {
    s_scale.length = 25;
    s_scale.speed = 981;
    s_allocator = new PxDefaultAllocator();
    s_physicsManager = new PhysicsManager();
    s_foundation =
        PxCreateFoundation(PX_PHYSICS_VERSION, *s_allocator, *s_physicsManager);

    if (!s_foundation) {
      delete s_allocator;
      s_allocator = nullptr;
      delete s_physicsManager;
      s_physicsManager = nullptr;
      return false;
    }

    s_pvd = PxCreatePvd(*s_foundation);
    s_pvdTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    s_pvd->connect(*s_pvdTransport, PxPvdInstrumentationFlag::eALL);
    s_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *s_foundation, PxTolerancesScale(), true, s_pvd);
    if (!s_physics) {
      s_pvd->release();
      s_pvd = nullptr;
      s_pvdTransport->release();
      s_pvdTransport = nullptr;
      s_foundation->release();
      s_foundation = nullptr;
      delete s_allocator;
      s_allocator = nullptr;
      delete s_physicsManager;
      s_physicsManager = nullptr;
      return false;
    }

    s_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *s_foundation, PxCookingParams(s_scale));
    if (!s_cooking) {
      s_physics->release();
      s_physics = nullptr;
      s_pvd->release();
      s_pvd = nullptr;
      s_pvdTransport->release();
      s_pvdTransport = nullptr;
      s_foundation->release();
      s_foundation = nullptr;
      delete s_allocator;
      s_allocator = nullptr;
      delete s_physicsManager;
      s_physicsManager = nullptr;
      return false;
    }

    if (!PxInitExtensions(*s_physics, s_pvd)) {
      s_cooking->release();
      s_cooking = nullptr;
      s_physics->release();
      s_physics = nullptr;
      s_pvd->release();
      s_pvd = nullptr;
      s_pvdTransport->release();
      s_pvdTransport = nullptr;
      s_foundation->release();
      s_foundation = nullptr;
      delete s_allocator;
      s_allocator = nullptr;
      delete s_physicsManager;
      s_physicsManager = nullptr;
      return false;
    }

    s_init = true;
  }

  return true;
}

void PhysicsManager::Destroy() { 
  if (s_init) {
    s_cooking->release();
    s_cooking = nullptr;
    s_physics->release();
    s_physics = nullptr;
    s_pvd->release();
    s_pvd = nullptr;
    s_pvdTransport->release();
    s_pvdTransport = nullptr;
    s_foundation->release();
    s_foundation = nullptr;
    delete s_allocator;
    s_allocator = nullptr;
    delete s_physicsManager;
    s_physicsManager = nullptr;
    s_init = false;
  }
}

PhysicsManager::~PhysicsManager() {}
PhysicsManager::PhysicsManager() {}

void PhysicsManager::reportError(PxErrorCode::Enum code, const char* message,
                                 const char* file, int line) {
  if (code == PxErrorCode::eDEBUG_INFO) {
    char buffer[1024];
    sprintf(buffer, "%s\n", message);
    std::cout << buffer << std::endl;
  } else {
    const char* errorCode = NULL;

    switch (code) {
      case PxErrorCode::eINVALID_PARAMETER:
        errorCode = "invalid parameter";
        break;
      case PxErrorCode::eINVALID_OPERATION:
        errorCode = "invalid operation";
        break;
      case PxErrorCode::eOUT_OF_MEMORY:
        errorCode = "out of memory";
        break;
      case PxErrorCode::eDEBUG_INFO:
        errorCode = "info";
        break;
      case PxErrorCode::eDEBUG_WARNING:
        errorCode = "warning";
        break;
      case PxErrorCode::ePERF_WARNING:
        errorCode = "performance warning";
        break;
      case PxErrorCode::eABORT:
        errorCode = "abort";
        break;
      default:
        errorCode = "unknown error";
        break;
    }

    PX_ASSERT(errorCode);
    if (errorCode) {
      char buffer[1024];
      sprintf(buffer, "%s (%d) : %s : %s\n", file, line, errorCode, message);

      std::cout << buffer << std::endl;

      // in debug builds halt execution for abort codes
      PX_ASSERT(code != PxErrorCode::eABORT);

      // in release builds we also want to halt execution
      // and make sure that the error message is flushed
      while (code == PxErrorCode::eABORT) {
        std::cout << buffer << std::endl;
      }
    }
  }
}