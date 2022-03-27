#ifndef TESTRENDERABLE_H
#define TESTRENDERABLE_H
#include <stdint.h>

#include "Material.h"
#include "Renderable.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace px::rendering {
class TestRenderable : public Renderable {
 public:
  TestRenderable();
  virtual ~TestRenderable();
  bool preRender(float deltaTime, const glm::vec3& cameraPos,
                 const glm::vec3& cameraRotation);
  bool shouldDrop() const;
  void setDropFlag();
  glm::mat4 getTransform() const;
  Material* getMaterial() const;
  void setPosition(glm::vec3 position);
  void render() const;

 private:
  Material* _material;
  uint32_t _vertexBufferId;
  uint32_t _vertexArrayId;
  uint32_t _indexBufferId;
  glm::vec3 _position;
  glm::vec3 _rotation;
  bool _drop;
};
}  // namespace px::rendering
#endif  // !TESTRENDERABLE_H