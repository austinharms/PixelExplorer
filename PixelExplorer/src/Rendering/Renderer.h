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
  void removeRenderable(uint32_t id);
  void drawFrame();
  bool getKeyPressed(int32_t key);
  void setFPSLimit(int32_t limit);
  void setCursorHidden(bool hidden);

  void removeRenderable(Renderable* renderable) {
    removeRenderable(renderable->getId());
  }

  int getFPSLimit() const { return _FPSLimit; }

  int getFPS() const { return _FPS; }

  float getDeltaTime() const { return _deltaTime; }

  bool getCursorHidden() const { return _cursorHidden; }

  bool getWindowOpen() const { return !glfwWindowShouldClose(_window); }

  void setPosition(glm::vec3 position) {
    std::lock_guard<std::mutex> drawLock(_renderLock);
    _position = position;
  }

  glm::vec3 getPosition() const { return _position; }

  void setRotation(glm::vec2 rotation) {
    std::lock_guard<std::mutex> drawLock(_renderLock);
    _rotation = rotation;
    updateForwardVector();
  }

  void setRotation(glm::vec3 rotation) {
    std::lock_guard<std::mutex> drawLock(_renderLock);
    _rotation = rotation;
    updateForwardVector();
  }

  glm::vec2 getRotation() const { return _rotation; }

  glm::vec2 getRotationVec3() const { return glm::vec3(_rotation, 0); }

  void setTransform(glm::vec3 position, glm::vec3 rotation) {
    std::lock_guard<std::mutex> drawLock(_renderLock);
    _position = position;
    if ((glm::vec2)rotation != _rotation) {
      updateForwardVector();
      _rotation = rotation;
    }
  }

  void setTransform(glm::vec3 position, glm::vec2 rotation) {
    std::lock_guard<std::mutex> drawLock(_renderLock);
    _position = position;
    if (rotation != _rotation) {
      updateForwardVector();
      _rotation = rotation;
    }
  }

  glm::vec3 getForward() const { return _forward; }

  double getCursorX() const { return _cursorX; }

  double getCursorY() const { return _cursorY; }

  double getCursorChangeX() const { return _cursorChangeX; }

  double getCursorChangeY() const { return _cursorChangeY; }

 private:
  void updateForwardVector();

  std::mutex _renderLock;  // locks for _renderableObjects list
  std::list<Renderable*> _renderableObjects;
  glm::mat4 _projection;
  glm::vec3 _position;
  glm::vec3 _forward;
  glm::vec2 _rotation;
  GLFWwindow* _window;
  uint32_t _FPSLimit;
  uint32_t _FPS;
  uint32_t _frameCounter;
  float _deltaTime;
  double _lastFrame;
  double _FPSTimer;
  double _cursorX;
  double _cursorY;
  double _cursorChangeX;
  double _cursorChangeY;
  bool _cursorHidden;

  static void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                         GLenum severity, GLsizei length,
                                         const GLchar* message,
                                         const void* userParam);
};
#endif  // !RENDERER_H
