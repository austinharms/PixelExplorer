#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Material.h"
#include "RefCounted.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
namespace px::rendering {
class Renderable : public RefCounted {
 public:
  virtual ~Renderable() {}
  virtual bool shouldDrop() const = 0;
  virtual Material* getMaterial() const = 0;
  // Should update and return mesh visibility
  virtual bool preRender(float deltaTime, const glm::vec3& cameraPos,
                         const glm::vec3& cameraRotation) = 0;
  virtual glm::mat4 getTransform() const = 0;
  virtual void render() const = 0;
};
}  // namespace px::rendering
#endif  // !RENDERABLE_H
