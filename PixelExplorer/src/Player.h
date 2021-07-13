#pragma once
#include "Renderer.h"
#include "RefCounted.h"

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
class Player : public virtual RefCounted {
 public:
  Player(Renderer* renderer);
  virtual ~Player();
  void update();
  glm::vec3 getPosition() const { return _position; }

 private:
  Renderer* _renderer;
  glm::vec3 _position;
  //glm::vec3 _rotation;
  float _mouseSpeed = 1.5f;
  glm::mat4 _viewMatrix;
  float _horizontalAngle = 3.14f;
  float _verticalAngle = 0.0f;
  float _moveSpeed = 50.0f;
};