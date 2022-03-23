#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <mutex>

#include "Chunk.fwd.h"
#include "ChunkManager.fwd.h"
#include "Direction.h"
#include "PxRigidStatic.h"
#include "RefCounted.h"
#include "ChunkRenderMesh.h"

class Chunk : public RefCounted {
 public:
  enum class Status : uint8_t {
    CREATED = 0,
    UNLOADED = 1,
    LOADING = 2,
    UNLOADING = 3,
    LOADED = 4,
    UPDATING = 5
  };

  static const int32_t CHUNK_SIZE = 25;
  static const int32_t LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
  static const int32_t BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;

  Chunk(ChunkRenderMesh* renderMesh);
  virtual ~Chunk();
  void UpdateMesh();
  void UpdateAdjacents();
  void Unload();
  void SetPosition(glm::vec3 position);
  glm::vec3 GetPosition() const;
  const Status GetStatus() const;
  void SetManager(ChunkManager* mgr);
  ChunkRenderMesh* GetRenderMesh() const;

 private:
  physx::PxRigidStatic* _physxActor;
  ChunkManager* _mgr;
  Chunk* _adjacentChunks[Direction::DIRECTION_COUNT];
  ChunkRenderMesh* _renderMesh;
  Block _blocks[BLOCK_COUNT];
  glm::vec<3, int32_t> _position;
  std::mutex _blockMutex;
};
#endif  // !CHUNK_H
