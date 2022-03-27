#include "WorldScene.h"
#include "World.h"

namespace px::game::world {
px::game::world::WorldScene::WorldScene(World* world) {
  world->grab();
  _world = world;
}

WorldScene::~WorldScene() { _world->drop(); }
}  // namespace px::game::world