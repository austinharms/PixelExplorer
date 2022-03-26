#include "PhysicsScene.h"

#include "Logger.h"
#include "PhysicsBase.h"
#include "StaticPhysicsObject.h"

PhysicsScene::PhysicsScene(PhysicsBase* base) {
  base->grab();
  _base = base;

  physx::PxSceneDesc desc(base->_pxScale);
  desc.gravity = physx::PxVec3(0, -9.81f, 0);
  desc.cpuDispatcher = base->_pxDispatcher;
  desc.filterShader = base->_pxDefaultFilter;
  _pxScene = base->_pxPhysics->createScene(desc);
  if (_pxScene == nullptr) Logger::error("Failed to Create PhysX Scene");
}

StaticPhysicsObject* PhysicsScene::createStaticObject(
    const glm::vec3& position, DataBuffer<float>* vertices,
    DataBuffer<uint32_t>* indices, const float vertexStride) {
  return new StaticPhysicsObject(this, position, vertices, indices, vertexStride);
}

PhysicsScene::~PhysicsScene() {
  _base->drop();
  _pxScene->release();
}
