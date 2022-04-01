#include "World.h"

#include "WorldScene.h"

namespace px::game::world {
World::World() { _blockSet = new chunk::BlockSet(); }
World::~World() { _blockSet->drop(); }
}  // namespace px::game::world
