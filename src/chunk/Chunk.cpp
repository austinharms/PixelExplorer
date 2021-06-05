#include "Chunk.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <list>

Chunk::Chunk(glm::vec<3, int> pos, Material* material)
    : _position(pos), _blocks(nullptr), _mesh(nullptr) {
  //_mesh->setTransform(glm::translate(
  //    glm::mat4(1.0f),
  //    glm::vec3(pos.x * c_size, pos.y * c_size, pos.z * c_size)));
  _mesh = new Mesh();
  _mesh->setMaterial(material);
}

Chunk::~Chunk() {
  if (_blocks != nullptr) free(_blocks);
  _mesh->drop();
}

void Chunk::generateChunk() {
  _blocks = (Block**)malloc(sizeof(Block*) * BLOCK_COUNT);
  if (_blocks == nullptr) return;
  for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
    _blocks[i] = Block::getBlock(0);
  }
}

void Chunk::updateMesh() {
  unsigned int vertexCount = 0;
  unsigned int indexCount = 0;
  std::list<float> vertices;
  std::list<float> uvs;
  std::list<unsigned short> indices;
  for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
    float x = (int)(i % CHUNK_SIZE);
    float y = (int)(i/LAYER_SIZE);
    float z = (int)((int)(i/CHUNK_SIZE) % CHUNK_SIZE);
    if (_blocks[i] == nullptr) continue;
    for (char f = 0; f < 6; ++f) {
      if ((f == Block::FRONT && z != 0 && _blocks[i - CHUNK_SIZE] != nullptr && !_blocks[i - CHUNK_SIZE]->getTransparent()) || 
          (f == Block::BACK && z != CHUNK_SIZE - 1 && _blocks[i + CHUNK_SIZE] != nullptr && !_blocks[i + CHUNK_SIZE]->getTransparent()) || 
          (f == Block::LEFT && x != 0 && _blocks[i - 1] != nullptr && !_blocks[i - 1]->getTransparent())) continue;
      Block::BlockFace* face = _blocks[i]->getBlockFace((Block::Face)f);
      for (unsigned char v = 0; v < face->vertexCount; ++v) {
        vertices.push_back(face->vertices[v * 3] + x);
        vertices.push_back(face->vertices[v * 3 + 1] + y);
        vertices.push_back(face->vertices[v * 3 + 2] - z);
        uvs.push_back(face->uvs[v * 2]);
        uvs.push_back(face->uvs[v * 2 + 1]);
      }
      for (unsigned char in = 0; in < face->indexCount; ++in) {
        indices.push_back(vertexCount + face->indices[in]);
      }
      vertexCount += face->vertexCount;
      indexCount += face->indexCount;
    }
  }

  _mesh->setVertexCount(vertexCount);
  _mesh->setIndexCount(indexCount);

  std::list<float>::iterator vert = vertices.begin();
  std::list<float>::iterator uvMap = uvs.begin();
  for (unsigned int i = 0; i < vertexCount; ++i) {
    float x = *vert;
    vert++;
    float y = *vert;
    vert++;
    float z = *vert;
    vert++;
    _mesh->setVertexPosition(i, x, y, z);
    x = *uvMap;
    uvMap++;
    y = *uvMap;
    uvMap++;
    _mesh->setVertexUV(i, x, y);
  }

  std::list<unsigned short>::iterator index = indices.begin();
  for (unsigned int i = 0; i < indexCount; ++i) {
    _mesh->setIndex(i, *index);
    index++;
  }

  _mesh->updateBuffers();
}
