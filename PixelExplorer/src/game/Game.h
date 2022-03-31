#ifndef GAME_H
#define GAME_H

#include "RefCounted.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsBase.h"
#include "physics/PhysicsBase.h"
#include "rendering/Renderer.h"

namespace px::game {
class Game : public RefCounted {
 public:
  Game(rendering::Renderer* renderer);
  virtual ~Game();
  void start();

 private:
  rendering::Renderer* _renderer;
};
}  // namespace px
#endif  // !GAME_H
