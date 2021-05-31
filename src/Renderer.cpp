#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer()
    : _projection(glm::perspective(45.0f, 800.0f / 600.0f, 0.0f, 10.0f)),
      _view(glm::lookAt(glm::vec3(2.5f, 2.5f, 2.0f),
                        glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 0.0f, 1.0f))), _activeShader(nullptr) {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(GLErrorCallback, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Renderer::~Renderer() {
  if (_activeShader != nullptr) _activeShader->drop();
}

void Renderer::useShader(Shader* s) {
  if (_activeShader != nullptr && s->getGLID() == _activeShader->getGLID()) return;
  if (_activeShader != nullptr) _activeShader->drop();
  s->grab();
  _activeShader = s;
  s->bind();  
}

void Renderer::drawMesh(Mesh* mesh) {
  mesh->grab();
  mesh->bind();
  glDrawElements(GL_TRIANGLES, mesh->getVertexBuffer()->getIndexCount(),
                 mesh->getVertexBuffer()->getIndexType(), nullptr);
  mesh->drop();
}

void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}