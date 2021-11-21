#ifndef RENDER_MATERIAL_H
#define RENDER_MATERIAL_H

#include "RefCounted.h"
#include "Shader.h"

class Material : public virtual RefCounted {
 public:
  Material(Shader* shader) : _id(++s_idCounter), _shader(nullptr) {
    setShader(shader);
  }

  virtual ~Material() {
    if (_shader != nullptr) _shader->drop();
  }

  void setShader(Shader* s) {
    assert(s->isValid());
    if (_shader != nullptr) _shader->drop();
    _shader = s;
    if (_shader == nullptr) return;
    _shader->grab();
  }

  Shader* getShader() const { return _shader; }

  unsigned int getId() const { return _id; }

  virtual void onPostBind(){} //Should be called after the attached shader is bound if the material has not already been bound

  static Material* getDefault() {
    if (s_default == nullptr) s_default = loadDefaultMaterial();
    return s_default;
  }

 private:
  static unsigned int s_idCounter;
  static Material* s_default;
  unsigned int _id;
  Shader* _shader;
  static Material* loadDefaultMaterial();
};
#endif  // !RENDER_MATERIAL_H
