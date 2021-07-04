#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <iostream>

bool Renderer::s_windowSizeChange = false;

Renderer::Renderer(int width, int height, const char* title,
                   Material* defaultMaterial)
    : _FOV(40.0f),
      _projection(glm::perspective(
          glm::radians(40.0f), (float)width / (float)height, 0.1f, 1000.0f)),
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
      _lastFPSFrame(0),
      _defaultMaterial(nullptr) {
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
  glEnable(GL_DEPTH_TEST);
  glDebugMessageCallback(Renderer::GLErrorCallback, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glfwSwapInterval(0);
  glfwSetFramebufferSizeCallback(_window, Renderer::windowResizeEvent);
  setDefaultMaterial(defaultMaterial);
  _renderInit = true;
}

Renderer::~Renderer() {
  if (_boundShader != nullptr) _boundShader->drop();
  if (_boundMaterial != nullptr) _boundMaterial->drop();
  if (_defaultMaterial != nullptr) _defaultMaterial->drop();
  for (Mesh* m : _meshes) {
    m->drop();
  }
  glfwTerminate();
}

void Renderer::useShader(Shader* s) {
  if (_boundShader != nullptr && s->getGLID() == _boundShader->getGLID())
    return;
  if (_boundShader != nullptr) _boundShader->drop();
  _boundShader = s;
  if (_boundShader == nullptr) return;
  _boundShader->grab();
  _boundShader->bind();
}

void Renderer::useMaterial(Material* m) {
  if (_boundMaterial != nullptr && m->getID() == _boundMaterial->getID())
    return;
  if (_boundMaterial != nullptr) _boundMaterial->drop();
  _boundMaterial = m;
  if (_boundMaterial == nullptr) return;
  _boundMaterial->grab();
  if (_boundMaterial->hasShader()) {
    useShader(_boundMaterial->getShader());
  } else {
    useShader(_defaultMaterial->getShader());
  }

  if (_boundMaterial->hasTexture()) {
    _boundMaterial->bindTexture(0);
    _boundShader->setUniform1i("u_Texture", 0);
    _boundShader->setUniform1i("u_UseTexture", 1);
  } else {
    _boundShader->setUniform1i("u_UseTexture", 0);
  }
  _boundShader->setUniformv4("u_Color", _boundMaterial->getColor());
}

void Renderer::setDefaultMaterial(Material* m) {
  if (_defaultMaterial != nullptr) _defaultMaterial->drop();
  _defaultMaterial = m;
  if (_defaultMaterial == nullptr) return;
  _defaultMaterial->grab();
}

void Renderer::addMesh(Mesh* mesh) {
  std::lock_guard<std::mutex> locker(_meshListLock);
  mesh->grab();
  _meshes.emplace_back(mesh);
}

void Renderer::removeMesh(unsigned int id) {
  std::lock_guard<std::mutex> locker(_meshListLock);
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
    clock_t c = std::clock() / CLOCKS_PER_SEC;
    std::cout << "FPS: " << _avgFPS << " Time: " << c << "s" << std::endl;
  }
  _lastFrame = currentFrame;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glm::mat4 vp = _projection * _view;

  {
    std::lock_guard<std::mutex> locker(_meshListLock);
    std::list<Mesh*>::iterator iter = _meshes.begin();
    std::list<Mesh*>::iterator end = _meshes.end();
    while (iter != end) {
      Mesh* m = *iter;
      if (!m->getRendererDropFlag()) {
        drawMesh(m, &vp);
        ++iter;
      } else {
        iter = _meshes.erase(iter);
        m->drop();
      }
    }
  }

  if (_boundMaterial != nullptr) _boundMaterial->drop();

  if (_boundShader != nullptr) {
    _boundShader->unbind();
    _boundShader->drop();
  }

  _boundMaterial = nullptr;
  _boundShader = nullptr;
  glfwSwapBuffers(_window);
  glfwPollEvents();
  ++_frameCount;
}

void Renderer::hideCursor(bool hidden) {
  _cursorHidden = hidden;
  if (hidden) {
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else {
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

void Renderer::getCursorPosition(double& x, double& y) {
  glfwGetCursorPos(_window, &x, &y);
}

void Renderer::setCursorPosition(double x, double y) {
  glfwSetCursorPos(_window, x, y);
}

void Renderer::drawMesh(Mesh* mesh, glm::mat4* vp) {
  mesh->updateTransfrom(_deltaTime);
  if (!mesh->getMeshVisible()) return;
  if (mesh->hasMaterial()) {
    useMaterial(mesh->getMaterial());
  } else {
    useMaterial(_defaultMaterial);
  }

  mesh->bind();
  glm::mat4 mvp = *vp * mesh->getTransform();
  _boundShader->setUniformm4("u_MVP", mvp);
  if (mesh->hasMaterial()) useMaterial(mesh->getMaterial());
  unsigned int indexCount = mesh->getIndexCount();
  unsigned int indexType = mesh->getIndexType();
  glDrawElements(GL_TRIANGLES, indexCount, indexType, nullptr);
  mesh->unbindLock();
}

void Renderer::updateWindowSize() {
  glfwGetFramebufferSize(_window, &_windowWidth, &_windowHeight);
  Renderer::s_windowSizeChange = false;
  if (_windowWidth == 0 || _windowHeight == 0) return;
  _projection = glm::perspective(glm::radians(_FOV),
                                 (float)_windowWidth / (float)_windowHeight,
                                 0.1f, 1000.0f);
  glViewport(0, 0, _windowWidth, _windowHeight);
}

void Renderer::windowResizeEvent(GLFWwindow* window, int width, int height) {
  Renderer::s_windowSizeChange = true;
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