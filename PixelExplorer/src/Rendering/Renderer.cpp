#include "Renderer.h"

#include <assert.h>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"

Renderer::Renderer(int width, int height, const char* title, int FPSLimit)
    : _FPSLimit(FPSLimit),
      _projection(glm::perspective(
          glm::radians(40.0f), (float)width / (float)height, 0.1f, 1000.0f)),
      _view(glm::mat4(1.0f)) {
  assert(glfwInit());
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  _window = glfwCreateWindow(width, height, title, NULL, NULL);
  assert(_window);
  glfwMakeContextCurrent(_window);
  assert(glewInit() == GLEW_OK);
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEPTH_TEST);
  glDebugMessageCallback(Renderer::GLErrorCallback, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glfwSwapInterval(_FPSLimit);
  // glfwSetFramebufferSizeCallback(_window, Renderer::windowResizeEvent);
}

void Renderer::addRenderable(Renderable* renderable) {}

void Renderer::removeRenderable(unsigned short id) {}

void Renderer::drawFrame() {
  std::lock_guard<std::mutex> drawLock(_renderLock);
  for (Renderable* r : _renderableObjects) drawRenderable(r);
}

void Renderer::drawRenderable(Renderable* renderable) {
  if (renderable->onPreRender(_deltaTime, nullptr, nullptr)) {
    renderable->getMaterial(); //Should use Material
    renderable->getTransform(); //Should calculate MVP and assign to shader in material
    renderable->onRender();
  }
}

Renderer::~Renderer() {
  for (Renderable* r : _renderableObjects) r->drop();
  glfwTerminate();
}

void GLAPIENTRY Renderer::GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                          GLenum severity, GLsizei length,
                                          const GLchar* message,
                                          const void* userParam) {
  if (type != 0x8251)
    fprintf(stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
            severity, message);
}