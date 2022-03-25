#include "PhysicsBase.h"

#include "Logger.h"
#include "PhysicsScene.h"

using namespace physx;

PhysicsBase* PhysicsBase::s_physicsBase = nullptr;

PhysicsBase::PhysicsBase() {
  _pxScale.length = 1;
  _pxScale.speed = 9.81f;
  _pxAllocator = new PxDefaultAllocator();
  _pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *_pxAllocator, *this);
  _pxPVD = PxCreatePvd(*_pxFoundation);
  _pxPVDTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 1000);
  _pxPVD->connect(*_pxPVDTransport, PxPvdInstrumentationFlag::eALL);
  _pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_pxFoundation,
                               PxTolerancesScale(), true, _pxPVD);
  PxCookingParams params(_pxScale);
  params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
  params.meshPreprocessParams |=
      PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
  _pxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *_pxFoundation, params);
  PxInitExtensions(*_pxPhysics, _pxPVD);
  _pxDefaultMaterial = _pxPhysics->createMaterial(1, 1, 1);
  _pxDispatcher = PxDefaultCpuDispatcherCreate(0);
  _pxDefaultFilter = PxDefaultSimulationFilterShader;
  Logger::Info("Created Physics Base");
}

PhysicsBase* PhysicsBase::CreatePhysicsBase() {
  if (s_physicsBase == nullptr) s_physicsBase = new PhysicsBase();

  return s_physicsBase;
}

void PhysicsBase::DropPhysicsBase() {
  if (s_physicsBase != nullptr) {
    if (s_physicsBase->getRefCount() > 1)
      Logger::Warn(
          "Dropping Physics Base, Warning Physics Base Still Referenced");
    s_physicsBase->drop();
    s_physicsBase = nullptr;
  }
}

PhysicsBase::~PhysicsBase() {
  delete _pxDispatcher;
  _pxDefaultMaterial->release();
  PxCloseExtensions();
  _pxCooking->release();
  _pxPhysics->release();
  _pxPVD->disconnect();
  _pxPVD->release();
  _pxPVDTransport->release();
  _pxFoundation->release();
  Logger::Info("Destroyed Physics Base");
}

physx::PxTriangleMesh* PhysicsBase::BakePxMesh(
    physx::PxTriangleMeshDesc& desc) {
  return _pxCooking->createTriangleMesh(
      desc, _pxPhysics->getPhysicsInsertionCallback());
}

physx::PxPhysics* PhysicsBase::GetPxPhysics() const {
  return _pxPhysics;
}

physx::PxMaterial* PhysicsBase::GetDefaultPxMaterial() const { return _pxDefaultMaterial; }

PhysicsScene* PhysicsBase::CreatePhysicsScene() {
  return new PhysicsScene(this);
}

void PhysicsBase::reportError(PxErrorCode::Enum code, const char* message,
                              const char* file, int line) {
  switch (code) {
    case PxErrorCode::eINVALID_PARAMETER:
      Logger::Error(std::string("PhysX Invalid Parameter: ") + message +
                    ", in File: " + file + " at Line: " + std::to_string(line));
      break;
    case physx::PxErrorCode::eINVALID_OPERATION:
      Logger::Error(std::string("PhysX Invalid Operation: ") + message +
                    ", in File: " + file + " at Line: " + std::to_string(line));
      break;
    case PxErrorCode::eOUT_OF_MEMORY:
      Logger::Error(std::string("PhysX Out of Memory: ") + message +
                    ", in File: " + file + " at Line: " + std::to_string(line));
      break;
    case PxErrorCode::eDEBUG_INFO:
      Logger::Debug(std::string("PhysX: ") + message);
      break;
    case PxErrorCode::eDEBUG_WARNING:
      Logger::Debug(std::string("PhysX Warning: ") + message +
                    ", in File: " + file + " at Line: " + std::to_string(line));
      break;
    case PxErrorCode::ePERF_WARNING:
      Logger::Warn(std::string("PhysX Performance: ") + message +
                   ", in File: " + file + " at Line: " + std::to_string(line));
      break;
    case PxErrorCode::eABORT:
      Logger::Fatal(std::string("PhysX: ") + message + ", in File: " + file +
                    " at Line: " + std::to_string(line));
      break;
    default:
      Logger::Error(std::string("PhysX: ") + message + ", in File: " + file +
                    " at Line: " + std::to_string(line));
      break;
  }
}
