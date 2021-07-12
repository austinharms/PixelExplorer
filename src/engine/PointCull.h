#pragma once
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "Mesh.h"

class PointCull {
 public:
  inline PointCull(glm::mat4 viewMatrix) {
    _camPos = -glm::vec3(viewMatrix[3]) * glm::mat3(viewMatrix);
    _camForward = -glm::normalize(glm::vec3(glm::inverse(viewMatrix)[2]));
  }
  inline ~PointCull() {}
  inline bool meshInView(Mesh* mesh) {
    glm::vec3 meshPos(mesh->getTransform()[3]);
    float baseDist = glm::distance(meshPos + (mesh->getSize()/2.0f), _camPos);
    if (baseDist <= 45) return true;
    glm::vec3 forwardPos = _camForward * baseDist + _camPos;
    float dist = glm::distance(meshPos, forwardPos);
    if (dist <= baseDist) return true;
    return false;
  }
 private:
  glm::vec3 _camPos;
  glm::vec3 _camForward;
};