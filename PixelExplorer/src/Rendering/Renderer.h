#ifndef RENDERER_H
#define RENDERER_H

#include <list>
#include <mutex>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "RefCounted.h"
#include "Renderable.h"
#include "glm/mat4x4.hpp"

class Renderer : public virtual RefCounted {
 public:
  Renderer(int width, int height, const char* title, int FPSLimit = 0);
  virtual ~Renderer();
  void addRenderable(Renderable* renderable);
  void removeRenderable(unsigned int id);
  void drawFrame();

  void removeRenderable(Renderable* renderable) {
    removeRenderable(renderable->getId());
  }

  int getFPSLimit() const { return _FPSLimit; }

  void setFPSLimit(int limit) { _FPSLimit = limit; }

  int getFPS() const { return _FPS; }

  float getDeltaTime() const { return _deltaTime; }

  void setCursorHidden(bool hidden) { _cursorHidden = hidden; }

  bool getCursorHidden() const { return _cursorHidden; }

  bool getWindowOpen() const { return !glfwWindowShouldClose(_window); }

 private:
  std::mutex _renderLock;  // locks for _renderableObjects list
  std::list<Renderable*> _renderableObjects;
  int _FPSLimit;
  glm::mat4 _projection;
  glm::mat4 _view;
  GLFWwindow* _window;
  int _FPS;
  int _frameCounter;
  float _deltaTime;
  double _lastFrame;
  double _FPSTimer;
  bool _cursorHidden;

  static void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                         GLenum severity, GLsizei length,
                                         const GLchar* message,
                                         const void* userParam);
};
#endif  // !RENDERER_H
