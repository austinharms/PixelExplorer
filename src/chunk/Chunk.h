#pragma once
#include <glm/vec3.hpp>

#include "RefCounted.h"
#include "Block.h"
#include "Mesh.h"
#include "Material.h"


class Chunk : public virtual RefCounted {
 public:
  Chunk(glm::vec<3, int> pos, Material* material);
  virtual ~Chunk();
  void generateChunk();
  void updateMesh();
  glm::vec<3, int> getPosition() const { return _position; }
  Mesh* getMesh() const { return _mesh; }
 private:
  const unsigned int CHUNK_SIZE = 40;
  const unsigned int LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
  const unsigned int BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;
  Block** _blocks;
  glm::vec<3,int> _position;
  Mesh* _mesh;
};