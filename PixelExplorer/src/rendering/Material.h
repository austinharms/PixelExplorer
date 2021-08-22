#ifndef RENDER_MATERIAL_H
#define RENDER_MATERIAL_H

#include "RefCounted.h"
#include "Shader.h"

class Material : public virtual RefCounted {
 public:
  Material(Shader* shader) : _id(++s_idCounter), _shader(nullptr) { setShader(shader); }

  virtual ~Material() {
    if (_shader != nullptr) _shader->drop();
  }

  void setShader(Shader* s) {
    if (_shader != nullptr) _shader->drop();
    _shader = s;
    if (_shader == nullptr) return;
    _shader->grab();
  }

  Shader* getShader() const { return _shader; }

  unsigned int getId() const { return _id; }

 private:
  static unsigned int s_idCounter;
  unsigned int _id;
  Shader* _shader;
};
#endif  // !RENDER_MATERIAL_H
