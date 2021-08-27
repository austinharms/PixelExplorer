#ifndef TESTRENDERABLE_H
#define TESTRENDERABLE_H

#include "GL/glew.h"
#include "RefCounted.h"
#include "Renderable.h"
#include "glm/mat4x4.hpp"

class TestRenderable : public virtual RefCounted, public virtual Renderable {
 public:
  TestRenderable();
  virtual ~TestRenderable();
  bool onPreRender(float deltaTime, float* cameraPos, float* cameraRotation);
  void onRender();

  glm::mat4 getTransform() { return _transform; }

 private:
  unsigned int _vertexBufferId;
  unsigned int _vertexArrayId;
  unsigned int _indexBufferId;
  glm::mat4 _transform;
};
#endif  // !TESTRENDERABLE_H