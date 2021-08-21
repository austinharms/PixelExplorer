#ifndef RENDERER_H
#define RENDERER_H

#include <list>
#include <mutex>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "RefCounted.h"
#include "Renderable.h"
#include "glm/mat4x4.hpp"

class Renderer : virtual RefCounted {
 public:
  Renderer();
  virtual ~Renderer();

 private:
  GLFWwindow* _window;
  std::mutex _renderLock;  // locks for _renderableObjects list
  std::list<Renderable*> _renderableObjects;
  glm::mat4 _projection;
  glm::mat4 _view;

  static void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                         GLenum severity, GLsizei length,
                                         const GLchar* message,
                                         const void* userParam);
};
#endif  // !RENDERER_H
