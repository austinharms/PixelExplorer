#pragma once
#include <glm/vec4.hpp>

#include "RefCounted.h"
#include "Texture.h"

class Material : public virtual RefCounted {
 public:
  Material() : _color(1.0f), _texture(nullptr), _id(s_nextID++) {}
  Material(float r, float g, float b, float a)
      : _color(1.0f), _texture(nullptr), _id(s_nextID++) {
    setColor(r, g, b, a);
  }
  Material(glm::vec4 color) : _color(1.0f), _texture(nullptr), _id(s_nextID++) {
    setColor(color);
  }
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
 private:
  static unsigned int s_nextID;
  unsigned int _id;
  Texture* _texture;
  glm::vec4 _color;
};