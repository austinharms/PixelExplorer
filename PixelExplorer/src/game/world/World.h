#ifndef WORLD_H
#define WORLD_H

#include "RefCounted.h"
#include "World.fwd.h"
#include "WorldScene.fwd.h"
#include "chunk/block/BlockSet.h"

namespace px::game::world {
class World : public RefCounted {
 public:
  World();
  virtual ~World();

 private:
  chunk::BlockSet* _blockSet;
};
}  // namespace px::game::world
#endif