#pragma once
#include <glm/vec3.hpp>
#include <mutex>

#include "Block.h"
#include "Material.h"
#include "Mesh.h"
#include "RefCounted.h"

class Chunk : public virtual RefCounted {
 public:
  const unsigned int CHUNK_SIZE = 25;
  const unsigned int LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
  const unsigned int BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;
  enum Status {
    UNLOADED = -1,
    UNLOADING = 0,
    LOADING = 1,
    LOADED = 2,
  };

  enum ChunkFace {
    FRONT = 0,
    TOP = 1,
    RIGHT = 2,
    BACK = 3,
    BOTTOM = 4,
    LEFT = 5
  };

  Chunk(glm::vec<3, int> pos);
  Chunk();
  virtual ~Chunk();
  void generateChunk();
  void updateMesh();
  void setChunkPosition(glm::vec<3, int> pos);
  void setAdjacentChunk(ChunkFace side, Chunk* chunk);
  void dropAdjacentChunks();

  glm::vec<3, int> getPosition() const { return _position; }

  Mesh* getMesh() const { return _mesh; }

  Status getStatus() {
    std::lock_guard<std::mutex> locker(_statusLock);
    return _status;
  }

  void setStatus(Status s) {
    std::lock_guard<std::mutex> locker(_statusLock);
    _status = s;
  }

  void setUnloadTime(unsigned long long int time) {
    std::lock_guard<std::mutex> locker(_unloadDelayLock);
    _unloadTime = time;
  }

  unsigned long long int getUnloadTime() {
    std::lock_guard<std::mutex> locker(_unloadDelayLock);
    return _unloadTime;
  }

  Chunk* getAdjacentChunk(ChunkFace side) {
    std::lock_guard<std::recursive_mutex> lock(_adjacentLock);
    return _adjacentChunks[(int)side];
  }

  static void setBlockMaterial(Material* mat) {
    if (_blockMaterial != nullptr) _blockMaterial->drop();
    _blockMaterial = mat;
    if (_blockMaterial != nullptr) _blockMaterial->grab();
  }

 private:
  static Material* _blockMaterial;
  Block** _blocks;
  glm::vec<3, int> _position;
  Mesh* _mesh;
  std::mutex _statusLock;
  Status _status;
  std::mutex _unloadDelayLock;
  unsigned long long int _unloadTime;
  std::recursive_mutex _adjacentLock;
  Chunk* _adjacentChunks[6] = {nullptr, nullptr, nullptr,
                               nullptr, nullptr, nullptr};
};