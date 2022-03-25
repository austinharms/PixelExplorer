#ifndef TESTRENDERABLE_H
#define TESTRENDERABLE_H

#include "GL/glew.h"
#include "RefCounted.h"
#include "Renderable.h"
#include "glm/mat4x4.hpp"
#include "glm/gtx/euler_angles.hpp"

class TestRenderable : public Renderable {
 public:
  TestRenderable();
  virtual ~TestRenderable();
  bool PreRender(float deltaTime, const glm::vec3& cameraPos,
                 const glm::vec3& cameraRotation);
  bool ShouldDrop() const;
  void SetDropFlag();
  glm::mat4 GetTransform() const;
  Material* GetMaterial() const;
  void SetPosition(glm::vec3 position);
  void Render() const;

 private:
  Material* _material;
  unsigned int _vertexBufferId;
  unsigned int _vertexArrayId;
  unsigned int _indexBufferId;
  glm::vec3 _position;
  glm::vec3 _rotation;
  bool _drop;
  //glm::mat4 _transform;
};
#endif  // !TESTRENDERABLE_H

/*
  virtual ~Renderable() {}
  virtual bool ShouldDrop() const = 0;
  virtual Material* GetMaterial() const = 0;
  // Should update and return mesh visibility
  virtual bool PreRender(float deltaTime, const glm::vec3& cameraPos,
                         const glm::vec3& cameraRotation) = 0;
  virtual glm::mat4 GetTransform() const = 0;
  virtual void Render() const = 0;
*/