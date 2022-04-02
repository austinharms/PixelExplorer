#include "PhysicsScene.h"

#include "Logger.h"
#include "PhysicsBase.h"
#include "StaticPhysicsObject.h"
namespace px::physics {
PhysicsScene::PhysicsScene() {
  PhysicsBase* base = &PhysicsBase::getInstance();
  physx::PxSceneDesc desc(*(base->getPxScale()));
  desc.gravity = physx::PxVec3(0, -9.81f, 0);
  desc.cpuDispatcher = base->getPxDispatcher();
  desc.filterShader = base->getPxSimulationFilter();
  _pxScene = base->getPxPhysics()->createScene(desc);
  if (_pxScene == nullptr) Logger::error("Failed to Create PhysX Scene");
}

PhysicsScene::~PhysicsScene() { _pxScene->release(); }

void PhysicsScene::insertObject(PhysicsObject* obj) {
  _pxScene->addActor(*(obj->getPxActor()));
}

void PhysicsScene::removeObject(PhysicsObject* obj) {
  _pxScene->removeActor(*(obj->getPxActor()));
}
}  // namespace px::physics
