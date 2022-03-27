#ifndef WORLDSCENE_H
#define WORLDSCENE_H

#include "RefCounted.h"
#include "WorldScene.fwd.h"
#include "World.fwd.h"
#include "chunk/Chunk.fwd.h"

namespace px::game::world {
class WorldScene : public RefCounted {
 public:
  WorldScene(World* world);
  virtual ~WorldScene();

 private:
  World* _world;
};
}  // namespace px::game::world
#endif  // !WORLDSCENE_H
