#ifndef WORLD_H
#define WORLD_H

#include "RefCounted.h"
#include "World.fwd.h"
#include "WorldScene.fwd.h"

namespace px::game::world {
class World : public RefCounted {
 public:
  static World* loadWorld();

  virtual ~World();

 private:
  World();
};
}
#endif