#pragma once
#include <glm/vec3.hpp>

#include "RefCounted.h"
#include "Block.h"
#include "Mesh.h"


class Chunk : public virtual RefCounted {
 public:
  Chunk(glm::vec<3, int> pos);
  virtual ~Chunk();
  void generateChunk();
  void updateMesh();
  glm::vec<3, int> getPosition() const { return _position; }
  Mesh* getMesh() const { return _mesh; }
 private:
  const unsigned int c_size = 20;
  const unsigned int c_layerSize = c_size * c_size;
  const unsigned int c_chunkSize = c_layerSize * c_size;
  Block** _blocks;
  glm::vec<3,int> _position;
  Mesh* _mesh;
  float next(std::list<float>::iterator i);
};