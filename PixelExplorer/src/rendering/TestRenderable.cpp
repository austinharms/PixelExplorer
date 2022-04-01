#include "TestRenderable.h"

#include "GL/glew.h"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"
#include "BasicMaterial.h"
namespace px::rendering {
TestRenderable::TestRenderable(Material* material) {
  _material = material;
  _material->grab();
  _position = glm::vec3(0);
  _rotation = glm::vec3(0);
  _drop = false;

  float vertices[24] = {
      -0.5f, -0.5f, -0.5f,  // 0
      0.5f,  -0.5f, -0.5f,  // 1
      0.5f,  0.5f,  -0.5f,  // 2
      -0.5f, 0.5f,  -0.5f,  // 3
      -0.5f, -0.5f, 0.5f,   // 4
      0.5f,  -0.5f, 0.5f,   // 5
      0.5f,  0.5f,  0.5f,   // 6
      -0.5f, 0.5f,  0.5f    // 7
  };

  unsigned short indices[36] = {
      0, 2, 1, 0, 3, 2,  // Front
      4, 5, 6, 4, 6, 7,  // Back
      0, 4, 7, 0, 7, 3,  // Left
      1, 6, 5, 1, 2, 6,  // Right
      3, 7, 6, 3, 6, 2,  // Top
      0, 5, 4, 0, 1, 5,  // Bottom
  };

  glGenVertexArrays(1, &_vertexArrayId);
  glBindVertexArray(_vertexArrayId);
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned short), indices,
               GL_STATIC_DRAW);
}

TestRenderable::~TestRenderable() {
  _material->drop();
  glDeleteVertexArrays(1, &_vertexArrayId);
  glDeleteBuffers(1, &_vertexBufferId);
  glDeleteBuffers(1, &_indexBufferId);
}

bool TestRenderable::preRender(float deltaTime,
                                      const glm::vec3& cameraPos,
                                      const glm::vec3& cameraRotation) {
  _rotation.x += deltaTime;
  _rotation.y += deltaTime;
  return !_drop;
}

bool TestRenderable::shouldDrop() const { return _drop; }

void TestRenderable::setDropFlag() { _drop = true; }

glm::mat4 TestRenderable::getTransform() const {
  glm::mat4 t(glm::eulerAngleYXZ(_rotation.y, _rotation.x, _rotation.z));
  t[3][0] = _position.x;
  t[3][1] = _position.y;
  t[3][2] = _position.z;
  return t;
}

Material* TestRenderable::getMaterial() const { return _material; }

void TestRenderable::setPosition(glm::vec3 position) {
  _position = position;
}

void TestRenderable::render() const {
  glBindVertexArray(_vertexArrayId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
}
}  // namespace px::rendering
