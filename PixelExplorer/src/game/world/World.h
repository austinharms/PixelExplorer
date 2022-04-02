#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>

#include "RefCounted.h"
#include "World.fwd.h"
#include "WorldScene.fwd.h"
#include "chunk/block/BlockSet.h"
#include "rendering/Renderer.h"
#include "chunk/ChunkRenderMesh.h"
#include "rendering/Material.h"

namespace px::game::world {
class World : public RefCounted {
 public:
  World(rendering::Renderer* renderer);
  virtual ~World();
  void update();
  chunk::ChunkRenderMesh* createChunkMesh();

 private:
  rendering::Renderer* _renderer;
  rendering::Material* _chunkMaterial;
  chunk::BlockSet* _blockSet;
  WorldScene** _scenes;
  uint8_t _sceneCount;
};
}  // namespace px::game::world
#endif