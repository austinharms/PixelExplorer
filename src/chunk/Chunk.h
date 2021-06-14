#pragma once
#include <glm/vec3.hpp>
#include <mutex>

#include "Block.h"
#include "Material.h"
#include "Mesh.h"
#include "RefCounted.h"

class Chunk : public virtual RefCounted {
 public:
  const unsigned int CHUNK_SIZE = 20;
  const unsigned int LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
  const unsigned int BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;
  enum Status {
    UNLOADED = 0,
    UNLOADING = 1,
    LOADING = 2,
    LOADED = 3,
  };

  Chunk(glm::vec<3, int> pos);
  virtual ~Chunk();
  void generateChunk();
  void updateMesh();

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
};