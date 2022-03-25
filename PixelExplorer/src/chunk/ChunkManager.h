#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <string>
#include <unordered_map>

#include "Chunk.h"
#include "RefCounted.h"
#include "glm/vec3.hpp"
#include "rendering/Renderer.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Chunk.fwd.h"
#include "ChunkManager.fwd.h"
#include "PxScene.h"
#include "glm/gtx/hash.hpp"

class ChunkManager : public RefCounted {
 public:
  ChunkManager(Renderer* renderer);
  virtual ~ChunkManager();
  void UnloadChunks();
  void LoadChunk(glm::vec<3, int32_t> pos);
  Chunk* GetChunk(glm::vec<3, int32_t> pos);
  void UpdateLoadedChunks();

 private:
  Renderer* _renderer;
  std::unordered_map<glm::vec<3, int32_t>, Chunk*> _chunks;
  physx::PxScene* _scene;
};

#endif