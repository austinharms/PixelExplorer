#include "Game.h"

#include "glm/vec3.hpp"
#include "rendering/TestRenderable.h"
#include "world/chunk/block/BlockDefinition.h"
#include "world/chunk/block/BlockShape.h"
#include "rendering/BasicMaterial.h"
namespace px::game {
Game::Game() {
  _renderer = new rendering::Renderer(1200, 800, "Pixel Explorer", 60);
  _renderer->setCursorHidden(true);
  chunk::BlockShape::loadShapes();
  chunk::BlockDefinition::loadDefinitions();
}

Game::~Game() {
  chunk::BlockDefinition::unloadDefinitions();
  chunk::BlockShape::unloadShapes();
  _renderer->drop();
}

void Game::start() {
  rendering::BasicMaterial* mat = new rendering::BasicMaterial(_renderer->loadShader("basic"));
  for (float x = 0; x < 25; ++x) {
    for (float y = 0; y < 25; ++y) {
      for (float z = 0; z < 25; ++z) {
        rendering::TestRenderable* t = new rendering::TestRenderable(mat);
        t->setPosition(glm::vec3(x,y,z) * 3.0f);
        _renderer->addRenderable(t);
        t->drop();
      }
    }
  }

  mat->drop();

  glm::vec3 camPos(12, 12, 75);
  glm::vec3 camRot(0, 0, 0);
  float moveSpeed = 50;

  while (_renderer->getWindowOpen()) {
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
}
}  // namespace px::game