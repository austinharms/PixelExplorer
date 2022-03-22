#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Material.h"
#include "RefCounted.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

class Renderable : public virtual RefCounted {
 public:
  virtual ~Renderable() {}
  virtual bool ShouldDrop() const = 0;
  virtual Material* GetMaterial() const = 0;
  // Should update and return mesh visibility
  virtual bool PreRender(float deltaTime, const glm::vec3& cameraPos,
                         const glm::vec3& cameraRotation) = 0;
  virtual glm::mat4 getTransform() const = 0;
  virtual void onRender() const = 0;
};
#endif  // !RENDERABLE_H
