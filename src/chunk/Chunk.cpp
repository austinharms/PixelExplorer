#include "Chunk.h"

#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <list>

#include "VertexBufferAttrib.h"

Material* Chunk::_blockMaterial = nullptr;

Chunk::Chunk(glm::vec<3, int> pos)
    : _position(pos),
      _blocks(nullptr),
      _mesh(nullptr),
      _unloadTime(0),
      _chunkModified(false) {
  _status = Status::LOADING;
  VertexBufferAttrib* repeatAttrib = new VertexBufferAttrib(2, GL_FLOAT);
  _mesh = new Mesh(&repeatAttrib, 1);
  repeatAttrib->drop();
  _mesh->setTransform(glm::translate(
      glm::mat4(1.0f),
      glm::vec3(pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE)));
  _mesh->setMaterial(Chunk::_blockMaterial);
}

Chunk::Chunk()
    : _position(0.0f),
      _blocks(nullptr),
      _mesh(nullptr),
      _unloadTime(0),
      _chunkModified(false) {
  _status = Status::LOADING;
  VertexBufferAttrib* repeatAttrib = new VertexBufferAttrib(2, GL_FLOAT);
  _mesh = new Mesh(&repeatAttrib, 1);
  repeatAttrib->drop();
  _mesh->setMaterial(Chunk::_blockMaterial);
}

Chunk::~Chunk() {
  if (_blocks != nullptr) free(_blocks);
  _mesh->setRendererDropFlag(true);
  _mesh->drop();
}

void Chunk::generateChunk() {
  std::lock_guard<std::mutex> lock(_blockLock);
  if (_blocks != nullptr) free(_blocks);
  _blocks = (Block**)malloc(sizeof(Block*) * BLOCK_COUNT);
  if (_blocks == nullptr) return;
  for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
    _blocks[i] = Block::getBlock(i % 4);
  }
  _blocks[LAYER_SIZE] = nullptr;
  setChunkModified(true);
}

void Chunk::updateMesh() {
  std::lock_guard<std::recursive_mutex> adjLock(_adjacentLock);
  std::lock_guard<std::mutex> blockLock(_blockLock);

  setChunkModified(false);

  unsigned int vertexCount = 0;
  unsigned int indexCount = 0;
  bool hasFrontChunk = getAdjacentChunk(FRONT) != nullptr &&
                       getAdjacentChunk(FRONT)->getStatus() >= LOADING;
  bool hasBackChunk = getAdjacentChunk(BACK) != nullptr &&
                      getAdjacentChunk(BACK)->getStatus() >= LOADING;
  bool hasLeftChunk = getAdjacentChunk(LEFT) != nullptr &&
                      getAdjacentChunk(LEFT)->getStatus() >= LOADING;
  bool hasRightChunk = getAdjacentChunk(RIGHT) != nullptr &&
                       getAdjacentChunk(RIGHT)->getStatus() >= LOADING;
  bool hasTopChunk = getAdjacentChunk(TOP) != nullptr &&
                      getAdjacentChunk(TOP)->getStatus() >= LOADING;
  bool hasBottomChunk = getAdjacentChunk(BOTTOM) != nullptr &&
                        getAdjacentChunk(BOTTOM)->getStatus() >= LOADING;

  for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
    float x = (int)(i % CHUNK_SIZE);
    float y = (int)(i / LAYER_SIZE);
    float z = (int)((int)(i / CHUNK_SIZE) % CHUNK_SIZE);
    if (_blocks[i] == nullptr) continue;
    for (char f = 0; f < 6; ++f) {
      if ((f == Block::FRONT && (z != 0 && _blocks[i - CHUNK_SIZE] != nullptr && _blocks[i - CHUNK_SIZE]->getFullBlockFace((Block::Face)f)) || z == 0 && hasFrontChunk && getAdjacentChunk(FRONT)->getBlockAtIndexUnsafe(LAYER_SIZE) != nullptr ) ||
          (f == Block::BACK && z != CHUNK_SIZE - 1 && _blocks[i + CHUNK_SIZE] != nullptr && _blocks[i + CHUNK_SIZE]->getFullBlockFace((Block::Face)f)) ||
          (f == Block::LEFT && x != 0 && _blocks[i - 1] != nullptr && _blocks[i - 1]->getFullBlockFace((Block::Face)f)) ||
          (f == Block::RIGHT && x != CHUNK_SIZE - 1 && _blocks[i + 1] != nullptr && _blocks[i + 1]->getFullBlockFace((Block::Face)f)) ||
          (f == Block::BOTTOM && y != 0 && _blocks[i - LAYER_SIZE] != nullptr && _blocks[i - LAYER_SIZE]->getFullBlockFace((Block::Face)f)) ||
          (f == Block::TOP && y != CHUNK_SIZE - 1 && _blocks[i + LAYER_SIZE] != nullptr && _blocks[i + LAYER_SIZE]->getFullBlockFace((Block::Face)f)))
        continue;
      Block::BlockFace* face = _blocks[i]->getBlockFace((Block::Face)f);
      vertexCount += face->vertexCount;
      indexCount += face->indexCount;
    }
  }

  _mesh->setVertexCount(vertexCount);
  _mesh->setIndexCount(indexCount);
  vertexCount = 0;
  indexCount = 0;

  for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
    float x = (int)(i % CHUNK_SIZE);
    float y = (int)(i / LAYER_SIZE);
    float z = (int)((int)(i / CHUNK_SIZE) % CHUNK_SIZE);
    if (_blocks[i] == nullptr) continue;
    for (char f = 0; f < 6; ++f) {
      if ((f == Block::FRONT && (z != 0 && _blocks[i - CHUNK_SIZE] != nullptr && _blocks[i - CHUNK_SIZE]->getFullBlockFace((Block::Face)f)) || z == 0 && hasFrontChunk && getAdjacentChunk(FRONT)->getBlockAtIndexUnsafe(LAYER_SIZE) != nullptr ) ||
          (f == Block::BACK && z != CHUNK_SIZE - 1 && _blocks[i + CHUNK_SIZE] != nullptr && _blocks[i + CHUNK_SIZE]->getFullBlockFace((Block::Face)f)) ||
          (f == Block::LEFT && x != 0 && _blocks[i - 1] != nullptr && _blocks[i - 1]->getFullBlockFace((Block::Face)f)) ||
          (f == Block::RIGHT && x != CHUNK_SIZE - 1 && _blocks[i + 1] != nullptr && _blocks[i + 1]->getFullBlockFace((Block::Face)f)) ||
          (f == Block::BOTTOM && y != 0 && _blocks[i - LAYER_SIZE] != nullptr && _blocks[i - LAYER_SIZE]->getFullBlockFace((Block::Face)f)) ||
          (f == Block::TOP && y != CHUNK_SIZE - 1 && _blocks[i + LAYER_SIZE] != nullptr && _blocks[i + LAYER_SIZE]->getFullBlockFace((Block::Face)f)))
        continue;
      Block::BlockFace* face = _blocks[i]->getBlockFace((Block::Face)f);
      for (unsigned char v = 0; v < face->vertexCount; ++v) {
        _mesh->setVertexPosition(vertexCount + v, face->vertices[v * 3] + x,
                                 face->vertices[v * 3 + 1] + y,
                                 face->vertices[v * 3 + 2] - z);
        _mesh->setVertexUV(vertexCount + v, face->uvs[v * 2],
                           face->uvs[v * 2 + 1]);
        _mesh->setAttribVec2(2, vertexCount + v, 1, 1);
      }
      for (unsigned char in = 0; in < face->indexCount; ++in) {
        _mesh->setIndex(indexCount + in, vertexCount + face->indices[in]);
      }
      vertexCount += face->vertexCount;
      indexCount += face->indexCount;
    }
  }

  _mesh->updateBuffers();
}

void Chunk::setChunkPosition(glm::vec<3, int> pos) {
  _position = pos;
  _mesh->setTransform(glm::translate(
      glm::mat4(1.0f),
      glm::vec3(pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE)));
  setChunkModified(true);
}

void Chunk::setAdjacentChunk(ChunkFace side, Chunk* chunk) {
  {
    std::lock_guard<std::recursive_mutex> lock(_adjacentLock);
    Chunk* curChunk = getAdjacentChunk(side);
    if (curChunk == chunk) return;
    if (curChunk != nullptr) curChunk->drop();
    if (chunk != nullptr) chunk->grab();
    _adjacentChunks[(int)side] = chunk;
    setChunkModified(true);
  }
}

void Chunk::dropAdjacentChunks() {
  for (char i = 0; i < 6; ++i) {
    Chunk* adjChunk = getAdjacentChunk((ChunkFace)i);
    if (adjChunk != nullptr)
      adjChunk->setAdjacentChunk((ChunkFace)(((int)i + 3) % 6), nullptr);
    setAdjacentChunk((ChunkFace)i, nullptr);
  }
  setChunkModified(true);
}

Chunk::ChunkFace Chunk::blockFaceToChunkFace(Block::Face face) {
  switch (face) {
    case Block::FRONT:
      return ChunkFace::FRONT;
    case Block::BACK:
      return ChunkFace::BACK;
    case Block::LEFT:
      return ChunkFace::LEFT;
    case Block::RIGHT:
      return ChunkFace::RIGHT;
    case Block::TOP:
      return ChunkFace::TOP;
    case Block::BOTTOM:
      return ChunkFace::BOTTOM;
  }
}
