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
  bool getKeyPressed(int32_t key);

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

  void setPosition(glm::vec3 position) {
    std::lock_guard<std::mutex> drawLock(_renderLock);
    _position = position;
  }

  glm::vec3 getPosition() const { return _position; }

  void setRotation(glm::vec3 rotation) {
    std::lock_guard<std::mutex> drawLock(_renderLock);
    _rotation = rotation;
  }

  glm::vec3 getRotation() const { return _rotation; }

  void setTransform(glm::vec3 position, glm::vec3 rotation) {
    std::lock_guard<std::mutex> drawLock(_renderLock);
    _position = position;
    _rotation = rotation;
  }

 private:
  std::mutex _renderLock;  // locks for _renderableObjects list
  std::list<Renderable*> _renderableObjects;
  int _FPSLimit;
  glm::mat4 _projection;
  glm::vec3 _position;
  glm::vec3 _rotation;
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
