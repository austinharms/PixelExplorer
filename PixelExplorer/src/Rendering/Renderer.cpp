#include "Renderer.h"

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/vec3.hpp"

Renderer::Renderer(int width, int height, const char* title, int FPSLimit)
    : _FPSLimit(FPSLimit),
      _projection(glm::perspective(
          glm::radians(40.0f), (float)width / (float)height, 0.1f, 1000.0f)),
      _FPS(0),
      _frameCounter(0),
      _cursorHidden(false),
      _deltaTime(0),
      _position(0),
      _rotation(0) {
  assert(glfwInit());
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  _window = glfwCreateWindow(width, height, title, NULL, NULL);
  assert(_window);
  glfwMakeContextCurrent(_window);
  assert(glewInit() == GLEW_OK);
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEPTH_TEST);
  glDebugMessageCallback(Renderer::GLErrorCallback, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glfwSwapInterval(_FPSLimit);
  // glfwSetFramebufferSizeCallback(_window, Renderer::windowResizeEvent);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Enable Wireframe
  // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); //Disable Wireframe
  _lastFrame = glfwGetTime();
  _FPSTimer = _lastFrame;
}

void Renderer::addRenderable(Renderable* renderable) {
  std::lock_guard<std::mutex> locker(_renderLock);
  renderable->grab();
  _renderableObjects.emplace_back(renderable);
}

void Renderer::removeRenderable(unsigned int id) {
  std::lock_guard<std::mutex> locker(_renderLock);
  std::remove_if(_renderableObjects.begin(), _renderableObjects.end(),
                 [&id](Renderable* renderable) {
                   if (renderable->getId() == id) {
                     renderable->drop();
                     return true;
                   }

                   return false;
                 });
}

void Renderer::drawFrame() {
  double currentFrame = glfwGetTime();
  _deltaTime = currentFrame - _lastFrame;
  _lastFrame = currentFrame;
  if (currentFrame - _FPSTimer >= 0.250) {
    _FPS = _frameCounter * 4;
    _frameCounter = 0;
    _FPSTimer = currentFrame;
    std::cout << "FPS: " << _FPS << std::endl;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  {
    Material* currentMaterial = nullptr;
    Shader* currentShader = nullptr;
    std::lock_guard<std::mutex> drawLock(_renderLock);
    std::list<Renderable*>::iterator iter = _renderableObjects.begin();
    std::list<Renderable*>::iterator end = _renderableObjects.end();
    glm::mat4 view(glm::eulerAngleYXZ(_rotation.y, _rotation.x, _rotation.z));
    view[3][0] = _position.x;
    view[3][1] = _position.y;
    view[3][2] = _position.z;
    glm::mat4 vp = _projection * view;
    while (iter != end) {
      Renderable* renderable = *iter;
      if (renderable->getRendererDropFlag()) {
        iter = _renderableObjects.erase(iter);
        renderable->drop();
      } else {
        if (renderable->onPreRender(_deltaTime, glm::value_ptr(_position),
                                    glm::value_ptr(_rotation))) {
          Material* renderableMaterial = renderable->getMaterial();
          if (renderableMaterial != currentMaterial) {
            if (currentMaterial != nullptr) currentMaterial->drop();
            renderableMaterial->grab();
            currentMaterial = renderableMaterial;
            Shader* renderableShader = renderableMaterial->getShader();
            if (renderableShader != currentShader) {
              if (currentShader != nullptr) {
                currentShader->unbind();
                currentShader->drop();
              }

              renderableShader->grab();
              currentShader = renderableShader;
              renderableShader->bind();
            }
            renderableMaterial->onPostBind();
          }

          currentShader->setUniformm4("u_MVP", vp * renderable->getTransform());
          renderable->onRender();
        }
        ++iter;
      }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (currentShader != nullptr) {
      currentShader->unbind();
      currentShader->drop();
    }

    if (currentMaterial != nullptr) currentMaterial->drop();
  }

  glfwSwapBuffers(_window);
  glfwPollEvents();
  ++_frameCounter;
}

bool Renderer::getKeyPressed(int32_t key)
{
    return glfwGetKey(_window, key) == GLFW_PRESS;
}


Renderer::~Renderer() {
  std::lock_guard<std::mutex> locker(_renderLock);
  for (Renderable* r : _renderableObjects) r->drop();
  glfwTerminate();
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