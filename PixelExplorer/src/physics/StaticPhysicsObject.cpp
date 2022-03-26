#include "StaticPhysicsObject.h"

#include "PhysicsBase.h"
#include "PhysicsScene.h"

StaticPhysicsObject::StaticPhysicsObject(PhysicsScene* scene,
                                         const glm::vec3& position,
                                         DataBuffer<float>* vertices,
                                         DataBuffer<uint32_t>* indices,
                                         const float vertexStride) {
  scene->grab();
  _scene = scene;
  _pxShape = nullptr;
  _pxStaticBody = _scene->_base->getPxPhysics()->createRigidStatic(
      physx::PxTransform(position.x, position.y, position.z));
  _scene->_pxScene->addActor(*_pxStaticBody);
}

void StaticPhysicsObject::updateMesh(DataBuffer<float>* vertices,
                                     DataBuffer<uint32_t>* indices,
                                     const float vertexStride) {
  physx::PxTriangleMeshDesc meshDesc;
  vertices->makeReadOnly();
  meshDesc.points.count = vertices->length / vertexStride;
  meshDesc.points.stride = sizeof(float) * vertexStride;
  meshDesc.points.data = vertices->buffer;

  indices->makeReadOnly();
  meshDesc.triangles.count = indices->length / 3;
  meshDesc.triangles.stride = sizeof(uint32_t) * 3;
  meshDesc.triangles.data = indices->buffer;

  physx::PxTriangleMesh* mesh = _scene->_base->bakePxMesh(meshDesc);
  physx::PxMeshScale scale(physx::PxVec3(1));
  const physx::PxTriangleMeshGeometry geom(mesh, scale);
  physx::PxPhysics* physics = _scene->_base->getPxPhysics();
  if (_pxShape != nullptr) {
    _pxStaticBody->detachShape(*_pxShape);
    _pxShape->release();
  }

  _pxShape = physics->createShape(
      geom, *(_scene->_base->getDefaultPxMaterial()), true);
  _pxStaticBody->attachShape(*_pxShape);
  mesh->release();
}

StaticPhysicsObject::~StaticPhysicsObject() {
  _pxShape->release();
  _pxStaticBody->release();
  _scene->drop();
}