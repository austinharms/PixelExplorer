#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <mutex>

#include "Chunk.fwd.h"
#include "ChunkManager.fwd.h"
#include "util/Direction.h"
#include "PxRigidStatic.h"
#include "RefCounted.h"
#include "ChunkRenderMesh.h"
#include "block/Block.h"
namespace px::game::chunk {
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
  void updateMesh();
  void updateAdjacents();
  void unload();
  void setPosition(glm::vec3 position);
  glm::vec3 getPosition() const;
  const Status getStatus() const;
  void setManager(ChunkManager* mgr);
  ChunkRenderMesh* getRenderMesh() const;

 private:
  ChunkManager* _mgr;
  Chunk* _adjacentChunks[util::Direction::DIRECTION_COUNT];
  physx::PxRigidStatic* _physxActor;
  ChunkRenderMesh* _renderMesh;
  std::mutex _blockMutex;
  Block _blocks[BLOCK_COUNT];
  glm::vec<3, int32_t> _position;
  Status _status;
};
}  // namespace px::game::chunk
#endif  // !CHUNK_H
