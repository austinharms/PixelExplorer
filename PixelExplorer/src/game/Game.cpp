#include "Game.h"

#include "rendering/TestRenderable.h"
#include "world/chunk/block/BlockDefinition.h"
#include "world/chunk/block/BlockShape.h"

#include "glm/vec3.hpp"
namespace px::game {
Game::Game(rendering::Renderer* renderer, physics::PhysicsBase* physicsBase) {
  _renderer = renderer;
  _renderer->grab();
  _physics = physicsBase;
  _physics->grab();

  chunk::BlockShape::loadShapes();
  chunk::BlockDefinition::loadDefinitions();
}

Game::~Game() {
  chunk::BlockDefinition::unloadDefinitions();
  chunk::BlockShape::unloadShapes();
  _physics->drop();
  _renderer->drop();
}

void Game::start() {
  rendering::TestRenderable* t = new rendering::TestRenderable();
  _renderer->addRenderable(t);
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
}