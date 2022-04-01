#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

#include <string>
#include <list>
#include <mutex>
#include <unordered_map>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "RefCounted.h"
#include "Renderable.h"
#include "glm/mat4x4.hpp"
#include "Renderer.fwd.h"
#include "Shader.fwd.h"
namespace px::rendering {
class Renderer : public RefCounted {
 public:
  Renderer(int32_t width, int32_t height, const char* title, float FOV,
           uint32_t FPSLimit = 0);

  virtual ~Renderer();
  void addRenderable(Renderable* renderable);
  void drawFrame();
  bool getKeyPressed(int32_t key);
  void setFPSLimit(int32_t limit);
  void setCursorHidden(bool hidden);
  void removeRenderable(Renderable* renderable);
  int getFPSLimit() const;
  int getFPS() const;
  float getDeltaTime() const;
  bool getCursorHidden() const;
  bool getWindowOpen() const;
  void setPosition(glm::vec3 position);
  glm::vec3 getPosition() const;
  void setRotation(glm::vec2 rotation);
  void setRotation(glm::vec3 rotation);
  glm::vec2 getRotation() const;
  glm::vec2 getRotationVec3() const;
  void setTransform(glm::vec3 position, glm::vec3 rotation);
  void setTransform(glm::vec3 position, glm::vec2 rotation);
  glm::vec3 getForward() const;
  double getCursorX() const;
  double getCursorY() const;
  double getCursorChangeX() const;
  double getCursorChangeY() const;
  Shader* loadShader(std::string shaderName);
  void removeShader(std::string shaderName);

 private:
  static void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                         GLenum severity, GLsizei length,
                                         const GLchar* message,
                                         const void* userParam);
  static void s_windowFocus(GLFWwindow* window, int focused);
  static void s_windowResize(GLFWwindow* window, int width, int height);
  void windowResize(int width, int height);
  void windowFocus(bool focused);
  void updateForwardVector();
  void updatePrjectionMatrix();

  std::mutex _renderLock;  // locks for _renderableObjects list
  std::list<Renderable*> _renderableObjects;
  std::unordered_map<std::string, Shader*> _shaders;
  glm::mat4 _projection;
  glm::vec3 _position;
  glm::vec3 _forward;
  glm::vec3 _rotation;
  GLFWwindow* _window;
  uint32_t _FPSLimit;
  uint32_t _FPS;
  uint32_t _frameCounter;
  float _deltaTime;
  float _FOV;
  double _lastFrame;
  double _FPSTimer;
  double _cursorX;
  double _cursorY;
  double _cursorChangeX;
  double _cursorChangeY;
  uint32_t _screenWidth;
  uint32_t _screenHeight;
  bool _cursorHidden;
  bool _cursorHiddenTarget;
  bool _windowActive;
};
}  // namespace px::rendering
#endif  // !RENDERER_H
