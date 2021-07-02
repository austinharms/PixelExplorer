#pragma once
#include <atomic>
#include <glm/vec3.hpp>
#include <mutex>

#include "Material.h"
#include "Mesh.h"
#include "RefCounted.h"
#include "block/Block.h"
#include "block/BlockBase.h"
#include "block/Blocks.h"
#include "generator/ChunkGenerator.h"

class Chunk : public virtual RefCounted {
 public:
  const static short CHUNK_VERSION = 0;
  const static int CHUNK_SIZE = 25;
  const static int LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
  const static int BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;
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
  void updateMesh();
  void setChunkPosition(glm::vec<3, int> pos);
  void setAdjacentChunk(ChunkFace side, Chunk* chunk);
  void dropAdjacentChunks();
  void saveChunk(const char* path);
  void loadChunk(const char* path, ChunkGenerator* gen);
  void setBlocks(Block* blocks);

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

  Block* getBlockUnsafe(unsigned int index) { return &(_blocks[index]); }

  void markChunkModified() { _chunkModified = true; }

  void markBlocksModified() { _blocksModified = true; }

  bool getChunkModified() const { return _chunkModified; }

  bool getBlocksModified() const { return _blocksModified; }

  static std::string posToString(glm::vec<3, int> pos) {
    return std::to_string(pos.x) + "-" + std::to_string(pos.y) + "-" +
           std::to_string(pos.z);
  }

 private:
  static Material* _blockMaterial;
  Mesh* _mesh;
  Block* _blocks;
  std::mutex _blockLock;
  std::mutex _statusLock;
  std::mutex _unloadDelayLock;
  std::recursive_mutex _adjacentLock;
  glm::vec<3, int> _position;
  Status _status;
  std::atomic<bool> _chunkModified;
  std::atomic<bool> _blocksModified;
  unsigned long long int _unloadTime;
  Chunk* _adjacentChunks[6] = {nullptr, nullptr, nullptr,
                               nullptr, nullptr, nullptr};
  bool drawBlockFace(Block* block, BlockBase::Face face);
  void deleteBlocks();
};