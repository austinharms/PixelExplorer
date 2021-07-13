#include "Player.h"

#include <GLFW/glfw3.h>

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

Player::Player(Renderer* renderer)
    : _renderer(renderer), _position(0.0f, 0.0f, 20.0f), _viewMatrix(1.0f) {//, _rotation(0.0f) {
  _renderer->grab();
}

Player::~Player() { _renderer->drop(); }

void Player::update() {
  double xpos, ypos;
  _renderer->getCursorPosition(xpos, ypos);
  int windowWidth, windowHeight;
  _renderer->getWindowSize(windowWidth, windowHeight);
  _renderer->setCursorPosition(windowWidth / 2, windowHeight / 2);
  _horizontalAngle +=
      _mouseSpeed * _renderer->getDeltaTime() * float(windowWidth / 2 - xpos);
  _verticalAngle +=
      _mouseSpeed * _renderer->getDeltaTime() * float(windowHeight / 2 - ypos);
  //_rotation.x += _mouseSpeed * _renderer->getDeltaTime() * float(windowWidth / 2 - xpos);
  //if (_rotation.x >= 1) _rotation.x -= 2;
  //if (_rotation.x < -1) _rotation.x += 2;
  //_rotation.y += _mouseSpeed * _renderer->getDeltaTime() * float(windowHeight / 2 - ypos);
  //if (_rotation.y >= 1) _rotation.y -= 2;
  //if (_rotation.y < -1) _rotation.y += 2;
  glm::vec3 direction(cos(_verticalAngle) * sin(_horizontalAngle),
                      sin(_verticalAngle),
                      cos(_verticalAngle) * cos(_horizontalAngle));
  glm::vec3 right = glm::vec3(sin(_horizontalAngle - 3.14f / 2.0f), 0,
                              cos(_horizontalAngle - 3.14f / 2.0f));
  glm::vec3 up = glm::cross(right, direction);
  if (_renderer->getKey(GLFW_KEY_UP) == GLFW_PRESS) {
    _position += direction * _renderer->getDeltaTime() * _moveSpeed;
  }
  // Move backward
  if (_renderer->getKey(GLFW_KEY_DOWN) == GLFW_PRESS) {
    _position -= direction * _renderer->getDeltaTime() * _moveSpeed;
  }
  // Strafe right
  if (_renderer->getKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
    _position += right * _renderer->getDeltaTime() * _moveSpeed;
  }
  // Strafe left
  if (_renderer->getKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
    _position -= right * _renderer->getDeltaTime() * _moveSpeed;
  }
  _viewMatrix = glm::lookAt(_position, _position + direction, up);
  _renderer->setCameraTransform(_viewMatrix);
  //std::cout << "Rot: " << _rotation.x << " " << _rotation.y << " " << _rotation.z << std::endl;
}
