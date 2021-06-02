#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <list>

#include "Mesh.h"
#include "RefCounted.h"
#include "Shader.h"

class Renderer : public virtual RefCounted {
 public:
  Renderer(int width, int height, const char* title);
  virtual ~Renderer();
  bool renderInit() const { return _renderInit; }
  void useShader(Shader* s);
  void addMesh(Mesh* mesh);
  void removeMesh(unsigned int id);
  void removeMesh(Mesh* mesh);
  bool windowOpen();
  void render();
  int getFPS() const { return _avgFPS; }
  float getDeltaTime() const { return _deltaTime; }
  void getWindowSize(int& width, int& height) {
    width = _windowWidth;
    height = _windowHeight;
  };

 private:
  GLFWwindow* _window;
  Shader* _activeShader;
  glm::mat4 _projection;
  glm::mat4 _view;
  std::list<Mesh*> _meshes;
  int _windowWidth;
  int _windowHeight;
  float _deltaTime;
  double _lastFrame;
  double _lastFPSFrame;
  int _avgFPS;
  int _frameCount;
  bool _renderInit;
  void drawMesh(Mesh* mesh);
  void updateWindowSize();
  static void windowResizeEvent(GLFWwindow* window, int width, int height);
  static bool s_windowSizeChange;
};

void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam);