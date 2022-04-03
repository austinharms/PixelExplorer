#include "Game.h"

#include "Logger.h"
#include "glm/vec3.hpp"
#include "rendering/BasicMaterial.h"
#include "rendering/TestRenderable.h"
#include "world/chunk/block/BlockDefinition.h"
#include "world/chunk/block/BlockShape.h"
namespace px::game {
Game::Game() {
  _renderer = new rendering::Renderer(1200, 800, "Pixel Explorer", 60);
  _loadedWorld = nullptr;
}

Game::~Game() {
    _renderer->unload();
  if (!_renderer->drop())
    Logger::warn("Rendere still referenced after game destroyed");
}

void Game::start() {
  _loadedWorld = new world::World(_renderer);
  glm::vec3 camPos(12, 12, 75);
  glm::vec3 camRot(0, 0, 0);
  float moveSpeed = 50;

  while (_renderer->getWindowOpen()) {
    _loadedWorld->update();
    if (_renderer->getKeyPressed(GLFW_KEY_W)) {
      camPos += _renderer->getDeltaTime() * _renderer->getForward() * moveSpeed;
    } else if (_renderer->getKeyPressed(GLFW_KEY_S)) {
      camPos -= _renderer->getDeltaTime() * _renderer->getForward() * moveSpeed;
    }

    if (_renderer->getKeyPressed(GLFW_KEY_A)) {
      camPos -= _renderer->getDeltaTime() *
                glm::cross(_renderer->getForward(), glm::vec3(0, 1, 0)) *
                moveSpeed;
    } else if (_renderer->getKeyPressed(GLFW_KEY_D)) {
      camPos += _renderer->getDeltaTime() *
                glm::cross(_renderer->getForward(), glm::vec3(0, 1, 0)) *
                moveSpeed;
    }

    if (_renderer->getKeyPressed(GLFW_KEY_SPACE)) {
      camPos.y += _renderer->getDeltaTime() * moveSpeed;
    } else if (_renderer->getKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
      camPos.y -= _renderer->getDeltaTime() * moveSpeed;
    }

    camRot.x += 0.001f * _renderer->getCursorChangeY();
    camRot.y += 0.001f * _renderer->getCursorChangeX();

    if (_renderer->getKeyPressed(GLFW_KEY_ESCAPE))
      _renderer->setCursorHidden(false);

    _renderer->setTransform(camPos, camRot);
    _renderer->drawFrame();
  }

  _loadedWorld->unload();
  _loadedWorld->drop();
  _loadedWorld = nullptr;
}
}  // namespace px::game