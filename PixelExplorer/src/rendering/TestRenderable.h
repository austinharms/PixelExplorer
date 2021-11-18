#ifndef TESTRENDERABLE_H
#define TESTRENDERABLE_H

#include "GL/glew.h"
#include "RefCounted.h"
#include "Renderable.h"
#include "glm/mat4x4.hpp"
#include "glm/gtx/euler_angles.hpp"

class TestRenderable : public virtual RefCounted, public virtual Renderable {
 public:
  TestRenderable();
  virtual ~TestRenderable();
  bool onPreRender(float deltaTime, float* cameraPos, float* cameraRotation);
  void onRender();
  void setPosition(glm::vec3 position) { _position = position; }

  glm::mat4 getTransform() {
    glm::mat4 t(glm::eulerAngleYXZ(_rotation.y, _rotation.x, _rotation.z));
    t[3][0] = _position.x;
    t[3][1] = _position.y;
    t[3][2] = _position.z;
    return t;
  }

 private:
  unsigned int _vertexBufferId;
  unsigned int _vertexArrayId;
  unsigned int _indexBufferId;
  glm::vec3 _position;
  glm::vec3 _rotation;
  //glm::mat4 _transform;
};
#endif  // !TESTRENDERABLE_H