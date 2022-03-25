#ifndef GAME_H
#define GAME_H
#include "rendering/Renderer.h"
#include "RefCounted.h"
#include "physics/PhysicsBase.h"
#include "chunk/block/BlockShape.h"
#include "chunk/block/BlockDefinition.h"

class Game : public RefCounted {
 public:
  Game(Renderer* renderer, PhysicsBase* physicsBase);
  virtual ~Game();

 private:
  Renderer* _renderer;
  PhysicsBase* _physics;
};
#endif  // !GAME_H
