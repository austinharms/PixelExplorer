#include "PhysicsMesh.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

PhysicsMesh::PhysicsMesh(VertexBufferAttrib* customAttribs[],
                         unsigned short attribCount, rp3d::PhysicsWorld* world,
                         rp3d::Transform rbTransform)
    : Mesh(customAttribs, attribCount), _world(world) {
  _rb = _world->createRigidBody(rbTransform);
}

PhysicsMesh::PhysicsMesh(rp3d::PhysicsWorld* world,
                         rp3d::Transform rbTransform)
    : Mesh(), _world(world) {
  _rb = _world->createRigidBody(rbTransform);
}

PhysicsMesh::~PhysicsMesh() { _world->destroyRigidBody(_rb); }

void PhysicsMesh::updateTransfrom(float dt) {
  float matrix[16]; 
  _rb->getTransform().getOpenGLMatrix(matrix);
  setTransform( glm::make_mat4(matrix));
}
