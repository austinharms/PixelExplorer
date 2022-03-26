#ifndef GAME_H
#define GAME_H
#include "chunk/block/BlockDefinition.h"
#include "chunk/block/BlockShape.h"
#include "common/RefCounted.h"
#include "physics/PhysicsBase.h"
#include "rendering/Renderer.h"

namespace px::game {
class Game : public RefCounted {
 public:
  Game(Renderer* renderer, PhysicsBase* physicsBase);
  virtual ~Game();
  void start();

 private:
  Renderer* _renderer;
  PhysicsBase* _physics;
};
}  // namespace px
#endif  // !GAME_H
