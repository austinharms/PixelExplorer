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
      _chunkModified(false),
      _chunkSaveRequired(false) {
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
      _chunkModified(false),
      _chunkSaveRequired(false) {
  _status = Status::LOADING;
  VertexBufferAttrib* repeatAttrib = new VertexBufferAttrib(2, GL_FLOAT);
  _mesh = new Mesh(&repeatAttrib, 1);
  repeatAttrib->drop();
  _mesh->setMaterial(Chunk::_blockMaterial);
}

Chunk::~Chunk() {
  if (_blocks != nullptr) {
    for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
      if (_blocks[i] != nullptr) delete _blocks[i];
    }

    free(_blocks);
  }
  _mesh->setRendererDropFlag(true);
  _mesh->drop();
}

void Chunk::generateChunk() {
  std::lock_guard<std::mutex> lock(_blockLock);
  if (_blocks != nullptr) free(_blocks);
  _blocks = (Block**)malloc(sizeof(Block*) * BLOCK_COUNT);
  if (_blocks == nullptr) return;
  for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
    _blocks[i] = new Block(i % 4);
  }

  setChunkModified(true);
}

void Chunk::updateMesh() {
  std::lock_guard<std::recursive_mutex> adjLock(_adjacentLock);
  std::lock_guard<std::mutex> blockLock(_blockLock);

  setChunkModified(false);

  unsigned int vertexCount = 0;
  unsigned int indexCount = 0;
  bool hasFrontChunk = getAdjacentChunk(FRONT) != nullptr &&
                       getAdjacentChunk(FRONT)->getStatus() == LOADED;
  bool hasBackChunk = getAdjacentChunk(BACK) != nullptr &&
                      getAdjacentChunk(BACK)->getStatus() == LOADED;
  bool hasLeftChunk = getAdjacentChunk(LEFT) != nullptr &&
                      getAdjacentChunk(LEFT)->getStatus() == LOADED;
  bool hasRightChunk = getAdjacentChunk(RIGHT) != nullptr &&
                       getAdjacentChunk(RIGHT)->getStatus() == LOADED;
  bool hasTopChunk = getAdjacentChunk(TOP) != nullptr &&
                     getAdjacentChunk(TOP)->getStatus() == LOADED;
  bool hasBottomChunk = getAdjacentChunk(BOTTOM) != nullptr &&
                        getAdjacentChunk(BOTTOM)->getStatus() == LOADED;
  {
    auto addBlockFace = [&](BlockBase::BlockFace* face) {
      vertexCount += face->vertexCount;
      indexCount += face->indexCount;
    };

    for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
      float x = (int)(i % CHUNK_SIZE);
      float y = (int)(i / LAYER_SIZE);
      float z = (int)((int)(i / CHUNK_SIZE) % CHUNK_SIZE);
      if (_blocks[i] == nullptr) continue;
      if (z == 0) {
        if (!hasFrontChunk ||
            drawBlockFace(getAdjacentChunk(FRONT)->getBlockUnsafe(
                              i + LAYER_SIZE - CHUNK_SIZE),
                          BlockBase::FRONT))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::FRONT));
      } else if (drawBlockFace(_blocks[i - CHUNK_SIZE], BlockBase::FRONT)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::FRONT));
      }

      if (z == CHUNK_SIZE - 1) {
        if (!hasBackChunk ||
            drawBlockFace(getAdjacentChunk(BACK)->getBlockUnsafe(
                              i - LAYER_SIZE + CHUNK_SIZE),
                          BlockBase::BACK))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::BACK));
      } else if (drawBlockFace(_blocks[i + CHUNK_SIZE], BlockBase::BACK)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::BACK));
      }

      if (x == 0) {
        if (!hasLeftChunk ||
            drawBlockFace(
                getAdjacentChunk(LEFT)->getBlockUnsafe(i + CHUNK_SIZE - 1),
                BlockBase::LEFT))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::LEFT));
      } else if (drawBlockFace(_blocks[i - 1], BlockBase::LEFT)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::LEFT));
      }

      if (x == CHUNK_SIZE - 1) {
        if (!hasRightChunk ||
            drawBlockFace(
                getAdjacentChunk(RIGHT)->getBlockUnsafe(i - CHUNK_SIZE + 1),
                BlockBase::RIGHT))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::RIGHT));
      } else if (drawBlockFace(_blocks[i + 1], BlockBase::RIGHT)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::RIGHT));
      }

      if (y == 0) {
        if (!hasBottomChunk ||
            drawBlockFace(getAdjacentChunk(BOTTOM)->getBlockUnsafe(
                              i + BLOCK_COUNT - LAYER_SIZE),
                          BlockBase::BOTTOM))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::BOTTOM));
      } else if (drawBlockFace(_blocks[i - LAYER_SIZE], BlockBase::BOTTOM)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::BOTTOM));
      }

      if (y == CHUNK_SIZE - 1) {
        if (!hasTopChunk || drawBlockFace(getAdjacentChunk(TOP)->getBlockUnsafe(
                                              i - BLOCK_COUNT + LAYER_SIZE),
                                          BlockBase::TOP))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::TOP));
      } else if (drawBlockFace(_blocks[i + LAYER_SIZE], BlockBase::TOP)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::TOP));
      }
    }
  }

  _mesh->setVertexCount(vertexCount);
  _mesh->setIndexCount(indexCount);
  vertexCount = 0;
  indexCount = 0;
  {
    auto addBlockFace = [&](BlockBase::BlockFace* face, float x, float y,
                            float z) {
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
    };

    for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
      float x = (int)(i % CHUNK_SIZE);
      float y = (int)(i / LAYER_SIZE);
      float z = (int)((int)(i / CHUNK_SIZE) % CHUNK_SIZE);
      if (_blocks[i] == nullptr) continue;
      if (z == 0) {
        if (!hasFrontChunk ||
            drawBlockFace(getAdjacentChunk(FRONT)->getBlockUnsafe(
                              i + LAYER_SIZE - CHUNK_SIZE),
                          BlockBase::FRONT))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::FRONT), x, y, z);
      } else if (drawBlockFace(_blocks[i - CHUNK_SIZE], BlockBase::FRONT)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::FRONT), x, y, z);
      }

      if (z == CHUNK_SIZE - 1) {
        if (!hasBackChunk ||
            drawBlockFace(getAdjacentChunk(BACK)->getBlockUnsafe(
                              i - LAYER_SIZE + CHUNK_SIZE),
                          BlockBase::BACK))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::BACK), x, y, z);
      } else if (drawBlockFace(_blocks[i + CHUNK_SIZE], BlockBase::BACK)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::BACK), x, y, z);
      }

      if (x == 0) {
        if (!hasLeftChunk ||
            drawBlockFace(
                getAdjacentChunk(LEFT)->getBlockUnsafe(i + CHUNK_SIZE - 1),
                BlockBase::LEFT))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::LEFT), x, y, z);
      } else if (drawBlockFace(_blocks[i - 1], BlockBase::LEFT)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::LEFT), x, y, z);
      }

      if (x == CHUNK_SIZE - 1) {
        if (!hasRightChunk ||
            drawBlockFace(
                getAdjacentChunk(RIGHT)->getBlockUnsafe(i - CHUNK_SIZE + 1),
                BlockBase::RIGHT))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::RIGHT), x, y, z);
      } else if (drawBlockFace(_blocks[i + 1], BlockBase::RIGHT)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::RIGHT), x, y, z);
      }

      if (y == 0) {
        if (!hasBottomChunk ||
            drawBlockFace(getAdjacentChunk(BOTTOM)->getBlockUnsafe(
                              i + BLOCK_COUNT - LAYER_SIZE),
                          BlockBase::BOTTOM))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::BOTTOM), x, y, z);
      } else if (drawBlockFace(_blocks[i - LAYER_SIZE], BlockBase::BOTTOM)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::BOTTOM), x, y, z);
      }

      if (y == CHUNK_SIZE - 1) {
        if (!hasTopChunk || drawBlockFace(getAdjacentChunk(TOP)->getBlockUnsafe(
                                              i - BLOCK_COUNT + LAYER_SIZE),
                                          BlockBase::TOP))
          addBlockFace(_blocks[i]->getBlockFace(BlockBase::TOP), x, y, z);
      } else if (drawBlockFace(_blocks[i + LAYER_SIZE], BlockBase::TOP)) {
        addBlockFace(_blocks[i]->getBlockFace(BlockBase::TOP), x, y, z);
      }
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

void Chunk::saveChunk(const char* dir) {
  
}

bool Chunk::drawBlockFace(Block* block, BlockBase::Face face) {
  if (block == nullptr) return true;
  return !block->getFullBlockFace(face);
}
