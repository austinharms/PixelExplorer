#pragma once
#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include "RefCounted.h"
#include "Shader.h"
#include "Mesh.h"

class Renderer : public virtual RefCounted {
 public:
  Renderer();
  virtual ~Renderer();
  void useShader(Shader* s);
  void drawMesh(Mesh* mesh);
 private:
  Shader* _activeShader;
  glm::mat4 _projection;
  glm::mat4 _view;
};

void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam);