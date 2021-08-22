#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Material.h"
#include "RefCounted.h"

class Renderable : public virtual RefCounted {
 public:
  Renderable(Material* material)
      : _visible(true),
        _dropFlag(false),
        _id(++s_idCounter),
        _material(nullptr) {
    setMaterial(material);
  }

  virtual ~Renderable() {}

  bool getVisible() const { return _visible; }

  unsigned short getId() const { return _id; }

  bool getRendererDropFlag() const { return _dropFlag; }

  void setRendererDropFlag(bool flag) { _dropFlag = flag; }

  void setMaterial(Material* material) {
    if (material != nullptr) material->drop();
    _material = material;
    _material->grab();
  }

  Material* getMaterial() const { return _material; }

  virtual bool onPreRender(
      float deltaTime, float* cameraPos,
      float* cameraRotation) = 0;  // Should update and return mesh visibility
  virtual void getTransform() = 0;
  virtual void onRender() = 0;

 private:
  static unsigned int s_idCounter;
  bool _visible;
  bool _dropFlag;
  unsigned int _id;
  Material* _material;
};
#endif  // !RENDERABLE_H
