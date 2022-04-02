#ifndef GAME_H
#define GAME_H

#include "RefCounted.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsBase.h"
#include "physics/PhysicsBase.h"
#include "rendering/Renderer.h"
#include "world/World.h"

namespace px::game {
class Game : public RefCounted {
 public:
  Game();
  virtual ~Game();
  void start();

 private:
  rendering::Renderer* _renderer;
  world::World* _loadedWorld;
};
}  // namespace px
#endif  // !GAME_H
