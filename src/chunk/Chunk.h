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

  bool getChunkModified() {
    std::lock_guard<std::mutex> lock(_modifiedLock);
    return _chunkModified;
  }

  void setChunkModified(bool modified) {
    std::lock_guard<std::mutex> lock(_modifiedLock);
    _chunkModified = modified;
  }

  Block* getBlockUnsafe(unsigned int index) {
    if (_blocks != nullptr) return _blocks[index];
    return nullptr;
  }

 private:
  static Material* _blockMaterial;
  Status _status;
  Mesh* _mesh;
  Block** _blocks;
  std::mutex _blockLock;
  std::mutex _statusLock;
  std::mutex _unloadDelayLock;
  std::recursive_mutex _adjacentLock;
  glm::vec<3, int> _position;
  bool _chunkModified;
  unsigned long long int _unloadTime;
  std::mutex _modifiedLock;
  Chunk* _adjacentChunks[6] = {nullptr, nullptr, nullptr,
                               nullptr, nullptr, nullptr};
  bool drawBlockFace(Block* block, Block::Face face);
  static ChunkFace blockFaceToChunkFace(Block::Face face);
};