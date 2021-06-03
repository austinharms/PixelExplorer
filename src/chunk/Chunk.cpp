#include "Chunk.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <list>

Chunk::Chunk(glm::vec<3, int> pos)
    : _position(pos), _blocks(nullptr), _mesh(new Mesh()) {
  _mesh->setTransform(glm::translate(
      glm::mat4(1.0f),
      glm::vec3(pos.x * c_size, pos.y * c_size, pos.z * c_size)));
}

Chunk::~Chunk() {
  if (_blocks != nullptr) free(_blocks);
}

void Chunk::generateChunk() {
  _blocks = (Block**)malloc(sizeof(Block*) * c_chunkSize);
  if (_blocks == nullptr) return;
  for (unsigned int i = 0; i < c_chunkSize; ++i) {
    _blocks[i] = Block::getBlock(0);
  }
}

void Chunk::updateMesh() {
  unsigned int vertexCount = 0;
  unsigned int indexCount = 0;
  std::list<float> vertices;
  std::list<float> uvs;
  std::list<short> indices;
  for (unsigned int i = 0; i < c_chunkSize; ++i) {
    if (_blocks[i] == nullptr) continue;
    for (char f = 0; f < 6; ++f) {
      Block::BlockFace face = _blocks[i]->getBlockFace((Block::Face)f);
      vertexCount += face.vertexCount;
      indexCount += face.indexCount;
      for (unsigned char v = 0; v < face.vertexCount * 5; ++v) {
        vertices.push_back(face.vertices[v]);
        vertices.push_back(face.vertices[v + 1]);
        vertices.push_back(face.vertices[v + 2]);
        uvs.push_back(face.uvs[v + 3]);
        uvs.push_back(face.uvs[v + 4]);
      }
      for (unsigned char in = 0; in < face.indexCount; ++in) {
        indices.push_back(face.indices[in]);
      }
    }
  }

  _mesh->setVertexCount(vertexCount);
  _mesh->setIndexCount(indexCount);

  std::list<float>::iterator vert = vertices.begin();
  std::list<float>::iterator uvMap = uvs.begin();
  for (unsigned int i = 0; i < vertexCount; ++i) {
    _mesh->setVertexPosition(i, next(vert), next(vert), next(vert));
    _mesh->setVertexUV(i, next(uvMap), next(uvMap));
  }

  std::list<short>::iterator index = indices.begin();
  for (unsigned int i = 0; i < indexCount; ++i) {
    _mesh->setIndex(i, *index);
    index++;
  }

  _mesh->updateBuffers();
}

float Chunk::next(std::list<float>::iterator i) {
  float val = *i;
  i++;
  return val;
}
