#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <list>
#include <mutex>

#include "Material.h"
#include "Mesh.h"
#include "RefCounted.h"
#include "Shader.h"

class Renderer : public virtual RefCounted {
 public:
  Renderer(int width, int height, const char* title,
           Material* defaultMaterial = nullptr);
  virtual ~Renderer();
  void setDefaultMaterial(Material* m);
  void addMesh(Mesh* mesh);
  void removeMesh(unsigned int id); //better to set mesh drop flag
  void removeMesh(Mesh* mesh); //better to set mesh drop flag
  bool windowOpen();
  void render();
  void hideCursor(bool hide);

  int getKey(int keyCode) { return glfwGetKey(_window, keyCode); }

  bool getCursorHidden() const { return _cursorHidden; }

  void getCursorPosition(double& x, double& y);

  void setCursorPosition(double x, double y);

  bool renderInit() const { return _renderInit; }

  int getFPS() const { return _avgFPS; }

  float getDeltaTime() const { return _deltaTime; }

  void getWindowSize(int& width, int& height) {
    width = _windowWidth;
    height = _windowHeight;
  };

  void setCameraTransform(glm::mat4 t) { _view = t; }

  glm::mat4 getCameraTransform() const { return _view; }

  float getFOV() const { return _FOV; }

  void setFOV(float FOV) {
    _FOV = FOV;
    updateWindowSize();
  }

 private:
  GLFWwindow* _window;
  Shader* _boundShader;
  Material* _boundMaterial;
  Material* _defaultMaterial;
  glm::mat4 _projection;
  glm::mat4 _view;
  std::mutex _meshListLock;
  std::list<Mesh*> _meshes;
  int _windowWidth;
  int _windowHeight;
  float _deltaTime;
  double _lastFrame;
  double _lastFPSFrame;
  int _avgFPS;
  int _frameCount;
  float _FOV;
  bool _renderInit;
  bool _cursorHidden;
  void drawMesh(Mesh* mesh, glm::mat4* vp);
  void updateWindowSize();
  void useMaterial(Material* m);
  void useShader(Shader* s);
  static void windowResizeEvent(GLFWwindow* window, int width, int height);
  static bool s_windowSizeChange;
  static void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                         GLenum severity, GLsizei length,
                                         const GLchar* message,
                                         const void* userParam);
};