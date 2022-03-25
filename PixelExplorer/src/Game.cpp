#include "Game.h"

Game::Game(Renderer* renderer, PhysicsBase* physicsBase) {
  _renderer = renderer;
  _renderer->grab();
  _physics = physicsBase;
  _physics->grab();

  BlockShape::LoadShapes();
  BlockDefinition::LoadDefinitions();
}

Game::~Game() {
  BlockDefinition::UnloadDefinitions();
  BlockShape::UnloadShapes();
  _physics->drop();
  _renderer->drop();
}
