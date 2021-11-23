#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Material.h"
#include "RefCounted.h"
#include "glm/mat4x4.hpp"

class Renderable : public virtual RefCounted {
 public:
  Renderable(Material* material)
      : _visible(true),
        _dropFlag(false),
        _id(++s_idCounter),
        _material(nullptr) {
    setMaterial(material);
  }

  virtual ~Renderable() {
    if (_material != nullptr) _material->drop();
  }

  bool getVisible() const { return _visible; }

  uint32_t getId() const { return _id; }

  bool getRendererDropFlag() const { return _dropFlag; }

  void setRendererDropFlag(bool flag) { _dropFlag = flag; }

  void setMaterial(Material* material) {
    if (_material != nullptr) _material->drop();
    _material = material;
    _material->grab();
  }

  Material* getMaterial() const { return _material; }

  virtual bool onPreRender(
      float deltaTime, float* cameraPos,
      float* cameraRotation) = 0;  // Should update and return mesh visibility
  virtual glm::mat4 getTransform() = 0;
  virtual void onRender() = 0;

 private:
  Material* _material;
  static uint32_t s_idCounter;
  uint32_t _id;

 protected:
  bool _visible;
  bool _dropFlag;
};
#endif  // !RENDERABLE_H
