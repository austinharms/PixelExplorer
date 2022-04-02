#ifndef WORLDSCENE_H
#define WORLDSCENE_H

#include <stdint.h>
#include <unordered_map>

#include "RefCounted.h"
#include "WorldScene.fwd.h"
#include "World.fwd.h"
#include "chunk/Chunk.fwd.h"
#include "physics/PhysicsScene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/gtx/hash.hpp"

namespace px::game::world {
class WorldScene : public RefCounted {
 public:
  WorldScene(World* world);
  virtual ~WorldScene();
  void update();
  void unload();
  bool getLoaded() const;

 private:
  World* _world;
  physics::PhysicsScene* _physicsScene;
  std::unordered_map<glm::ivec3, chunk::Chunk*> _loadedChunks;
  bool _loaded;
};
}  // namespace px::game::world
#endif  // !WORLDSCENE_H
