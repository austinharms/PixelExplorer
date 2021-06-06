#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

bool Renderer::s_windowSizeChange = false;

Renderer::Renderer(int width, int height, const char* title)
    : _projection(glm::perspective(glm::radians(45.0f),
                                   (float)width / (float)height, 0.1f, 100.0f)),
      _view(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f))),
      _boundShader(nullptr),
      _boundMaterial(nullptr),
      _avgFPS(0),
      _deltaTime(0),
      _lastFrame(0),
      _window(nullptr),
      _windowHeight(width),
      _windowWidth(height),
      _renderInit(false),
      _frameCount(0),
      _lastFPSFrame(0) {
  if (!glfwInit()) {
    std::cout << "Failed to init glfw" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  _window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!_window) {
    std::cout << "Failed to create glfw window" << std::endl;
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(_window);
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to init glew" << std::endl;
    return;
  }
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(GLErrorCallback, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glfwSwapInterval(0);
  glfwSetFramebufferSizeCallback(_window, Renderer::windowResizeEvent);
  _renderInit = true;
}

Renderer::~Renderer() {
  if (_boundShader != nullptr) _boundShader->drop();
  if (_boundMaterial != nullptr) _boundMaterial->drop();
  for (Mesh* m : _meshes) {
    m->drop();
  }
  glfwTerminate();
}

void Renderer::useShader(Shader* s) {
  if (_boundShader != nullptr && s->getGLID() == _boundShader->getGLID())
    return;
  if (_boundShader != nullptr) _boundShader->drop();
  s->grab();
  _boundShader = s;
  s->bind();
}

void Renderer::useMaterial(Material* m) {
  if (_boundMaterial != nullptr && m->getID() == _boundMaterial->getID())
    return;
  if (_boundMaterial != nullptr) _boundMaterial->drop();
  m->grab();
  _boundMaterial = m;
  if (_boundMaterial->hasTexture()) {
    _boundMaterial->bindTexture(0);
    _boundShader->setUniform1i("u_Texture", 0);
    _boundShader->setUniform1i("u_UseTexture", 1);
  } else {
    _boundShader->setUniform1i("u_UseTexture", 0);
  }
  _boundShader->setUniformv4("u_Color", _boundMaterial->getColor());
}

void Renderer::addMesh(Mesh* mesh) {
  mesh->grab();
  _meshes.push_back(mesh);
}

void Renderer::removeMesh(unsigned int id) {
  std::remove_if(_meshes.begin(), _meshes.end(), [&id](Mesh* mesh) {
    if (mesh->getID() == id) {
      mesh->drop();
      return true;
    }
    return false;
  });
}

void Renderer::removeMesh(Mesh* mesh) { removeMesh(mesh->getID()); }

bool Renderer::windowOpen() { return !glfwWindowShouldClose(_window); }

void Renderer::render() {
  if (Renderer::s_windowSizeChange) updateWindowSize();
  double currentFrame = glfwGetTime();
  _deltaTime = (currentFrame - _lastFrame);
  if (currentFrame - _lastFPSFrame >= 0.25) {
    _avgFPS = _frameCount * 4;
    _frameCount = 0;
    _lastFPSFrame = currentFrame;
    std::cout << "FPS: " << _avgFPS << std::endl;
  }
  _lastFrame = currentFrame;
  glClear(GL_COLOR_BUFFER_BIT);
  for (Mesh* m : _meshes) {
    drawMesh(m);
  }
  if (_boundMaterial != nullptr) _boundMaterial->drop();
  _boundMaterial = nullptr;
  glfwSwapBuffers(_window);
  glfwPollEvents();
  ++_frameCount;
}

void Renderer::drawMesh(Mesh* mesh) {
  mesh->updateTransfrom(_deltaTime);
  mesh->bind();
  glm::mat4 mvp = _projection * _view * mesh->getTransform();
  _boundShader->setUniformm4("u_MVP", mvp);
  if (mesh->getMaterial() != nullptr) useMaterial(mesh->getMaterial());
  glDrawElements(GL_TRIANGLES, mesh->getVertexBuffer()->getIndexCount(),
                 mesh->getVertexBuffer()->getIndexType(), nullptr);
}

void Renderer::updateWindowSize() {
  glfwGetFramebufferSize(_window, &_windowWidth, &_windowHeight);
  Renderer::s_windowSizeChange = false;
  _projection = glm::perspective(glm::radians(45.0f),
                                 (float)_windowWidth / (float)_windowHeight,
                                 0.1f, 100.0f);
}

void Renderer::windowResizeEvent(GLFWwindow* window, int width, int height) {
  Renderer::s_windowSizeChange = true;
}

void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}