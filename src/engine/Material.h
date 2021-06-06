#pragma once
#include <glm/vec4.hpp>

#include "RefCounted.h"
#include "Shader.h"
#include "Texture.h"

class Material : public virtual RefCounted {
 public:
  Material() : _color(1.0f), _texture(nullptr), _id(s_nextID++), _shader(nullptr) {}
  Material(glm::vec4 color, Texture* texture = nullptr,
           Shader* shader = nullptr)
      : _color(color), _texture(texture), _id(s_nextID++), _shader(shader) {}
  virtual ~Material() {
    if (_texture != nullptr) _texture->drop();
  }
  unsigned int getID() const { return _id; }
  void setTexture(Texture* t) {
    if (_texture != nullptr) _texture->drop();
    t->grab();
    _texture = t;
  }
  glm::vec4 getColor() const { return _color; }
  void setColor(float r, float g, float b, float a) {
    _color.r = r;
    _color.g = g;
    _color.b = b;
    _color.a = a;
  }
  void setColor(glm::vec4 color) { _color = color; }
  void bindTexture(unsigned int slot) { _texture->bind(slot); }
  bool hasTexture() const { return _texture != nullptr; }
  bool hasShader() const { return _shader != nullptr; }
  Shader* getShader() const { return _shader; }

 private:
  static unsigned int s_nextID;
  unsigned int _id;
  Texture* _texture;
  Shader* _shader;
  glm::vec4 _color;
};