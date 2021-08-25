#ifndef RENDER_MATERIAL_H
#define RENDER_MATERIAL_H

#include "RefCounted.h"
#include "Shader.h"

class Material : public virtual RefCounted {
 public:
  Material(Shader* shader) : _id(++s_idCounter), _shader(nullptr) {
    setShader(shader);
  }

  //Needed for DEFAULT Material
  Material(const Shader* shader)
      : _id(++s_idCounter), _shader(const_cast<Shader*>(shader)) {}

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

  const static Material* DEFAULT;

 private:
  static unsigned int s_idCounter;
  unsigned int _id;
  Shader* _shader;
  static Material* loadDefaultMaterial();
};
#endif  // !RENDER_MATERIAL_H
