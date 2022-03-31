#include "Renderer.h"

#include <iostream>

#include "Logger.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/vec3.hpp"
namespace px::rendering {
Renderer::Renderer(int32_t width, int32_t height, const char* title, float FOV,
                   uint32_t FPSLimit) {
  // Init Renderer Members
  _position = glm::vec3(0);
  _rotation = glm::vec3(0);
  _forward = glm::vec3(0);
  _screenHeight = height;
  _screenWidth = width;
  _FOV = FOV;
  _lastFrame = 0;
  _FPSTimer = 0;
  _FPS = 0;
  _frameCounter = 0;
  _deltaTime = 0;
  _cursorX = 0;
  _cursorY = 0;
  _cursorChangeX = 0;
  _cursorChangeY = 0;
  _windowActive = true;
  updatePrjectionMatrix();
  updateForwardVector();

  // Init GLFW/Render Window

  if (!glfwInit()) {
    const char* error = nullptr;
    Logger::fatal("Failed to initialize GLFW, Error Code: " +
                  std::to_string(glfwGetError(&error)) + error);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  _window = glfwCreateWindow(width, height, title, NULL, NULL);

  if (!_window) {
    const char* error = nullptr;
    Logger::fatal("Failed to create Window, Error Code: " +
                  std::to_string(glfwGetError(&error)) + error);
  }

  glfwMakeContextCurrent(_window);
  glfwSetWindowFocusCallback(_window, Renderer::s_windowFocus);
  glfwSetFramebufferSizeCallback(_window, Renderer::s_windowResize);
  glfwRequestWindowAttention(_window);

  // Need to do this after GLFW init as they depend on it
  setFPSLimit(FPSLimit);
  setCursorHidden(false);

  // Init OpenGL
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    Logger::fatal("Failed to initialize GLEW, Error Code: " +
                  std::to_string((uint32_t)err) +
                  std::string((const char*)glewGetErrorString(err)));
  }

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(Renderer::GLErrorCallback, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Enable Wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Disable Wireframe
  glfwSetWindowUserPointer(_window, this);
}

void Renderer::addRenderable(Renderable* renderable) {
  std::lock_guard<std::mutex> locker(_renderLock);
  renderable->grab();
  //if (_renderableObjects.begin() == _renderableObjects.end()) {
  //  _renderableObjects.emplace_back(renderable);
  //} else {
  //  uint32_t newShaderId = renderable->getMaterial()->getShader()->getGLID();
  //  uint32_t newMaterialId = renderable->getMaterial()->getId();
  //  for (std::list<Renderable*>::iterator it = _renderableObjects.begin();
  //       it != _renderableObjects.end(); ++it) {
  //    Renderable* other = *it;
  //    uint32_t otherShaderId = other->getMaterial()->getShader()->getGLID();
  //    if (newShaderId < otherShaderId) {
  //      _renderableObjects.insert(it, renderable);
  //      break;
  //    } else if (newShaderId == otherShaderId) {
  //      if (newMaterialId <= other->getMaterial()->getId()) {
  //        _renderableObjects.insert(it, renderable);
  //        break;
  //      }
  //    }
  //  }

    _renderableObjects.insert(_renderableObjects.end() , renderable);
  //}
}

void Renderer::removeRenderable(Renderable* renderable) {
  std::lock_guard<std::mutex> locker(_renderLock);
  std::remove_if(_renderableObjects.begin(), _renderableObjects.end(),
                 [&renderable](Renderable* renderable) {
                   if (renderable == renderable) {
                     renderable->drop();
                     return true;
                   }

                   return false;
                 });
}

void Renderer::drawFrame() {
  glfwMakeContextCurrent(_window);
  double currentFrame = glfwGetTime();
  _deltaTime = currentFrame - _lastFrame;
  _lastFrame = currentFrame;
  if (currentFrame - _FPSTimer >= 0.250) {
    _FPS = _frameCounter * 4;
    _frameCounter = 0;
    _FPSTimer = currentFrame;
    std::cout << "FPS: " << _FPS << std::endl;
  }

  if (_windowActive) {
    if (_cursorHidden) {
      glfwGetCursorPos(_window, &_cursorChangeX, &_cursorChangeY);
      glfwSetCursorPos(_window, 0, 0);
    } else {
      _cursorChangeX = _cursorX;
      _cursorChangeY = _cursorY;
      glfwGetCursorPos(_window, &_cursorX, &_cursorY);
      _cursorChangeX = _cursorX - _cursorChangeX;
      _cursorChangeY = _cursorY - _cursorChangeY;
    }
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  {
    Material* boundMaterial = nullptr;
    Shader* boundShader = nullptr;
    std::lock_guard<std::mutex> drawLock(_renderLock);
    std::list<Renderable*>::iterator iter = _renderableObjects.begin();
    std::list<Renderable*>::iterator end = _renderableObjects.end();

    glm::mat4 vp = _projection * glm::lookAt(_position, _position + _forward,
                                             glm::vec3(0, 1, 0));
    while (iter != end) {
      Renderable* renderable = *iter;
      if (renderable->shouldDrop()) {
        iter = _renderableObjects.erase(iter);
        renderable->drop();
        continue;
      }

      if (!renderable->preRender(_deltaTime, _position, _rotation)) continue;

      Material* renderableMaterial = renderable->getMaterial();
      if (renderableMaterial == nullptr) {
        iter = _renderableObjects.erase(iter);
        renderable->drop();
        Logger::error("Renderable returned NULL Material, Renderable removed");
        continue;
      }

      Shader* renderableShader = renderableMaterial->getShader();
      if (renderableShader == nullptr) {
        iter = _renderableObjects.erase(iter);
        renderable->drop();
        Logger::error(
            "Renderable Material returned NULL Shader, Renderable removed");
        continue;
      }

      if (renderableMaterial != boundMaterial) {
        if (renderableShader != boundShader) {
          renderableShader->bind();
          // dont grab shader due to grabing the parent renderable (too slow)
          boundShader = renderableShader;
        }

        renderableMaterial->bind();
        // dont grab material due to grabing the parent renderable (too slow)
        boundMaterial = renderableMaterial;
      }

      boundShader->setUniformm4("u_MVP", vp * renderable->getTransform());
      renderable->render();
      ++iter;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if (boundShader != nullptr) boundShader->unbind();
  }

  glfwSwapBuffers(_window);
  glfwPollEvents();
  ++_frameCounter;
}

bool Renderer::getKeyPressed(int32_t key) {
  return glfwGetKey(_window, key) == GLFW_PRESS;
}

void Renderer::setFPSLimit(int32_t limit) {
  glfwMakeContextCurrent(_window);
  _FPSLimit = limit;
  glfwSwapInterval(_FPSLimit);
}

void Renderer::setCursorHidden(bool hidden) {
  _cursorHiddenTarget = hidden;
  if (_windowActive) {
    _cursorHidden = _cursorHiddenTarget;
    if (_cursorHidden) {
      glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    glfwSetCursorPos(_window, 0, 0);
    _cursorChangeX = 0;
    _cursorChangeY = 0;
    _cursorX = 0;
    _cursorY = 0;
  }
}

Renderer::~Renderer() {
  std::lock_guard<std::mutex> locker(_renderLock);
  for (Renderable* r : _renderableObjects) r->drop();
  glfwDestroyWindow(_window);
}

void Renderer::updateForwardVector() {
  float cosY = cosf(_rotation.y * 0.5f);
  float sinY = sinf(_rotation.y * 0.5f);
  float cosX = cosf(_rotation.x * 0.5f);
  float sinX = sinf(_rotation.x * 0.5f);

  float x = cosX * cosY;
  float y = -sinX * sinY;
  float z = cosX * sinY;
  float w = sinX * cosY;

  _forward.x = 2 * (x * z + w * y);
  _forward.y = 2 * (y * z - w * x);
  _forward.z = 1 - 2 * (x * x + y * y);
}

void Renderer::updatePrjectionMatrix() {
  _projection = glm::perspective(glm::radians(_FOV),
                                 (float)_screenWidth / (float)_screenHeight,
                                 0.1f, 1000.0f);
}

void Renderer::windowResize(int width, int height) {
  std::cout << "Window Size Changed: " << width << " " << height << std::endl;
  _screenWidth = width;
  _screenHeight = height;
  updatePrjectionMatrix();
}

void Renderer::windowFocus(bool focused) {
  _windowActive = focused;
  if (_windowActive) {
    setCursorHidden(_cursorHiddenTarget);
  } else if (_cursorHidden) {
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    _cursorHidden = false;
  }
}

void GLAPIENTRY Renderer::GLErrorCallback(GLenum source, GLenum type, GLuint id,
                                          GLenum severity, GLsizei length,
                                          const GLchar* message,
                                          const void* userParam) {
  if (type != 0x8251) {
    if (type == GL_DEBUG_TYPE_ERROR) {
      Logger::error("GL Error: type = " + std::to_string(type) +
                    ", severity = " + std::to_string(severity) +
                    ", message = " + message);
    } else {
      Logger::warn("GL: type = " + std::to_string(type) + ", severity = " +
                   std::to_string(severity) + ", message = " + message);
    }
  }
}

void Renderer::s_windowFocus(GLFWwindow* window, int focused) {
  ((Renderer*)glfwGetWindowUserPointer(window))->windowFocus(focused);
}

void Renderer::s_windowResize(GLFWwindow* window, int width, int height) {
  ((Renderer*)glfwGetWindowUserPointer(window))->windowResize(width, height);
}

int Renderer::getFPSLimit() const { return _FPSLimit; }

int Renderer::getFPS() const { return _FPS; }

float Renderer::getDeltaTime() const { return _deltaTime; }

bool Renderer::getCursorHidden() const { return _cursorHidden; }

bool Renderer::getWindowOpen() const { return !glfwWindowShouldClose(_window); }

void Renderer::setPosition(glm::vec3 position) {
  std::lock_guard<std::mutex> drawLock(_renderLock);
  _position = position;
}

glm::vec3 Renderer::getPosition() const { return _position; }

void Renderer::setRotation(glm::vec2 rotation) {
  std::lock_guard<std::mutex> drawLock(_renderLock);
  _rotation = glm::vec3(rotation, 0);
  updateForwardVector();
}

void Renderer::setRotation(glm::vec3 rotation) {
  std::lock_guard<std::mutex> drawLock(_renderLock);
  _rotation = rotation;
  updateForwardVector();
}

glm::vec2 Renderer::getRotation() const { return _rotation; }

glm::vec2 Renderer::getRotationVec3() const { return _rotation; }

void Renderer::setTransform(glm::vec3 position, glm::vec3 rotation) {
  std::lock_guard<std::mutex> drawLock(_renderLock);
  _position = position;
  if (rotation != _rotation) {
    updateForwardVector();
    _rotation = rotation;
  }
}

void Renderer::setTransform(glm::vec3 position, glm::vec2 rotation) {
  std::lock_guard<std::mutex> drawLock(_renderLock);
  _position = position;
  if (rotation != (glm::vec2)_rotation) {
    updateForwardVector();
    _rotation = glm::vec3(rotation, 0);
  }
}

glm::vec3 Renderer::getForward() const { return _forward; }

double Renderer::getCursorX() const { return _cursorX; }

double Renderer::getCursorY() const { return _cursorY; }

double Renderer::getCursorChangeX() const { return _cursorChangeX; }

double Renderer::getCursorChangeY() const { return _cursorChangeY; }
}  // namespace px::rendering
