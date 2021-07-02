#include "Chunk.h"

#include <GL/glew.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <list>

#include "VertexBufferAttrib.h"

Material* Chunk::_blockMaterial = nullptr;

Chunk::Chunk(glm::vec<3, int> pos)
    : _position(pos),
      _blocks(nullptr),
      _mesh(nullptr),
      _unloadTime(0),
      _chunkModified(false),
      _blocksModified(false) {
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
      _blocksModified(false) {
  _status = Status::LOADING;
  VertexBufferAttrib* repeatAttrib = new VertexBufferAttrib(2, GL_FLOAT);
  _mesh = new Mesh(&repeatAttrib, 1);
  repeatAttrib->drop();
  _mesh->setMaterial(Chunk::_blockMaterial);
}

Chunk::~Chunk() {
  {
    std::lock_guard<std::mutex> lock(_blockLock);
    deleteBlocks();
  }
  _mesh->setRendererDropFlag(true);
  _mesh->drop();
}

void Chunk::setBlocks(Block* blocks) {
  std::lock_guard<std::mutex> lock(_blockLock);
  deleteBlocks();
  _blocks = blocks;
  _chunkModified = true;
  _blocksModified = true;
}

void Chunk::updateMesh() {
  auto t1 = std::chrono::high_resolution_clock::now();
  std::lock_guard<std::recursive_mutex> adjLock(_adjacentLock);
  std::lock_guard<std::mutex> blockLock(_blockLock);
  _mesh->lockMeshEditing();
  _chunkModified = false;

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
      if (_blocks[i] == 0) continue;
      BlockBase* block = Blocks::getBlock((int32_t)_blocks[i]);
      if (z == 0) {
        if (!hasFrontChunk ||
            drawBlockFace(getAdjacentChunk(FRONT)->getBlockUnsafe(
                              i + LAYER_SIZE - CHUNK_SIZE),
                          BlockBase::FRONT))
          addBlockFace(block->getBlockFace(BlockBase::FRONT));
      } else if (drawBlockFace(&_blocks[i - CHUNK_SIZE], BlockBase::FRONT)) {
        addBlockFace(block->getBlockFace(BlockBase::FRONT));
      }

      if (z == CHUNK_SIZE - 1) {
        if (!hasBackChunk ||
            drawBlockFace(getAdjacentChunk(BACK)->getBlockUnsafe(
                              i - LAYER_SIZE + CHUNK_SIZE),
                          BlockBase::BACK))
          addBlockFace(block->getBlockFace(BlockBase::BACK));
      } else if (drawBlockFace(&_blocks[i + CHUNK_SIZE], BlockBase::BACK)) {
        addBlockFace(block->getBlockFace(BlockBase::BACK));
      }

      if (x == 0) {
        if (!hasLeftChunk ||
            drawBlockFace(
                getAdjacentChunk(LEFT)->getBlockUnsafe(i + CHUNK_SIZE - 1),
                BlockBase::LEFT))
          addBlockFace(block->getBlockFace(BlockBase::LEFT));
      } else if (drawBlockFace(&_blocks[i - 1], BlockBase::LEFT)) {
        addBlockFace(block->getBlockFace(BlockBase::LEFT));
      }

      if (x == CHUNK_SIZE - 1) {
        if (!hasRightChunk ||
            drawBlockFace(
                getAdjacentChunk(RIGHT)->getBlockUnsafe(i - CHUNK_SIZE + 1),
                BlockBase::RIGHT))
          addBlockFace(block->getBlockFace(BlockBase::RIGHT));
      } else if (drawBlockFace(&_blocks[i + 1], BlockBase::RIGHT)) {
        addBlockFace(block->getBlockFace(BlockBase::RIGHT));
      }

      if (y == 0) {
        if (!hasBottomChunk ||
            drawBlockFace(getAdjacentChunk(BOTTOM)->getBlockUnsafe(
                              i + BLOCK_COUNT - LAYER_SIZE),
                          BlockBase::BOTTOM))
          addBlockFace(block->getBlockFace(BlockBase::BOTTOM));
      } else if (drawBlockFace(&_blocks[i - LAYER_SIZE], BlockBase::BOTTOM)) {
        addBlockFace(block->getBlockFace(BlockBase::BOTTOM));
      }

      if (y == CHUNK_SIZE - 1) {
        if (!hasTopChunk || drawBlockFace(getAdjacentChunk(TOP)->getBlockUnsafe(
                                              i - BLOCK_COUNT + LAYER_SIZE),
                                          BlockBase::TOP))
          addBlockFace(block->getBlockFace(BlockBase::TOP));
      } else if (drawBlockFace(&_blocks[i + LAYER_SIZE], BlockBase::TOP)) {
        addBlockFace(block->getBlockFace(BlockBase::TOP));
      }
    }
  }

  _mesh->setVertexCount(vertexCount);
  _mesh->setIndexCount(indexCount);
  vertexCount = 0;
  indexCount = 0;
  unsigned short vertexOffset = _mesh->getAttribOffset(0);
  unsigned short uvOffset = _mesh->getAttribOffset(1);
  unsigned short repeatOffset = _mesh->getAttribOffset(2);

  {
    auto addBlockFace = [&](BlockBase::BlockFace* face, float x, float y,
                            float z) {
      for (unsigned char v = 0; v < face->vertexCount; ++v) {
        _mesh->setRawAttribVec3(
            vertexOffset, vertexCount + v, face->vertices[v * 3] + x,
            face->vertices[v * 3 + 1] + y, face->vertices[v * 3 + 2] - z);
        _mesh->setRawAttribVec2(uvOffset, vertexCount + v, face->uvs[v * 2],
                                face->uvs[v * 2 + 1]);
        _mesh->setRawAttribVec2(repeatOffset, vertexCount + v, 1, 1);
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
      if (_blocks[i] == 0) continue;
      BlockBase* block = Blocks::getBlock((int32_t)_blocks[i]);
      if (z == 0) {
        if (!hasFrontChunk ||
            drawBlockFace(getAdjacentChunk(FRONT)->getBlockUnsafe(
                              i + LAYER_SIZE - CHUNK_SIZE),
                          BlockBase::FRONT))
          addBlockFace(block->getBlockFace(BlockBase::FRONT), x, y, z);
      } else if (drawBlockFace(&_blocks[i - CHUNK_SIZE], BlockBase::FRONT)) {
        addBlockFace(block->getBlockFace(BlockBase::FRONT), x, y, z);
      }

      if (z == CHUNK_SIZE - 1) {
        if (!hasBackChunk ||
            drawBlockFace(getAdjacentChunk(BACK)->getBlockUnsafe(
                              i - LAYER_SIZE + CHUNK_SIZE),
                          BlockBase::BACK))
          addBlockFace(block->getBlockFace(BlockBase::BACK), x, y, z);
      } else if (drawBlockFace(&_blocks[i + CHUNK_SIZE], BlockBase::BACK)) {
        addBlockFace(block->getBlockFace(BlockBase::BACK), x, y, z);
      }

      if (x == 0) {
        if (!hasLeftChunk ||
            drawBlockFace(
                getAdjacentChunk(LEFT)->getBlockUnsafe(i + CHUNK_SIZE - 1),
                BlockBase::LEFT))
          addBlockFace(block->getBlockFace(BlockBase::LEFT), x, y, z);
      } else if (drawBlockFace(&_blocks[i - 1], BlockBase::LEFT)) {
        addBlockFace(block->getBlockFace(BlockBase::LEFT), x, y, z);
      }

      if (x == CHUNK_SIZE - 1) {
        if (!hasRightChunk ||
            drawBlockFace(
                getAdjacentChunk(RIGHT)->getBlockUnsafe(i - CHUNK_SIZE + 1),
                BlockBase::RIGHT))
          addBlockFace(block->getBlockFace(BlockBase::RIGHT), x, y, z);
      } else if (drawBlockFace(&_blocks[i + 1], BlockBase::RIGHT)) {
        addBlockFace(block->getBlockFace(BlockBase::RIGHT), x, y, z);
      }

      if (y == 0) {
        if (!hasBottomChunk ||
            drawBlockFace(getAdjacentChunk(BOTTOM)->getBlockUnsafe(
                              i + BLOCK_COUNT - LAYER_SIZE),
                          BlockBase::BOTTOM))
          addBlockFace(block->getBlockFace(BlockBase::BOTTOM), x, y, z);
      } else if (drawBlockFace(&_blocks[i - LAYER_SIZE], BlockBase::BOTTOM)) {
        addBlockFace(block->getBlockFace(BlockBase::BOTTOM), x, y, z);
      }

      if (y == CHUNK_SIZE - 1) {
        if (!hasTopChunk || drawBlockFace(getAdjacentChunk(TOP)->getBlockUnsafe(
                                              i - BLOCK_COUNT + LAYER_SIZE),
                                          BlockBase::TOP))
          addBlockFace(block->getBlockFace(BlockBase::TOP), x, y, z);
      } else if (drawBlockFace(&_blocks[i + LAYER_SIZE], BlockBase::TOP)) {
        addBlockFace(block->getBlockFace(BlockBase::TOP), x, y, z);
      }
    }
  }

  _mesh->updateBuffers();
  _mesh->unlockMeshEditing();
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms_double = t2 - t1;
  std::cout << "Chunk update took: " << ms_double.count() << "ms" << std::endl;
}

void Chunk::setChunkPosition(glm::vec<3, int> pos) {
  _position = pos;
  _mesh->setTransform(glm::mat4(
      1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      (float)(pos.x * (int)CHUNK_SIZE), (float)(pos.y * (int)CHUNK_SIZE),
      (float)(pos.z * (int)CHUNK_SIZE), 1.0));
  _chunkModified = true;
}

void Chunk::setAdjacentChunk(ChunkFace side, Chunk* chunk) {
  {
    std::lock_guard<std::recursive_mutex> lock(_adjacentLock);
    Chunk* curChunk = getAdjacentChunk(side);
    if (curChunk == chunk) return;
    if (curChunk != nullptr) curChunk->drop();
    if (chunk != nullptr) chunk->grab();
    _adjacentChunks[(int)side] = chunk;
    _chunkModified = true;
  }
}

void Chunk::dropAdjacentChunks() {
  for (char i = 0; i < 6; ++i) {
    Chunk* adjChunk = getAdjacentChunk((ChunkFace)i);
    if (adjChunk != nullptr)
      adjChunk->setAdjacentChunk((ChunkFace)(((int)i + 3) % 6), nullptr);
    setAdjacentChunk((ChunkFace)i, nullptr);
  }
  _chunkModified = true;
}

void Chunk::saveChunk(const char* dir) {
  std::lock_guard<std::mutex> lock(_blockLock);
  if (_blocksModified) {
    _blocksModified = false;
    FILE* file = nullptr;
    std::string path = dir + posToString(_position / 100);
    std::filesystem::create_directory(path);
    path += "\\" + posToString(_position) + ".ch";
    if (fopen_s(&file, path.c_str(), "wb") == 0 && file != 0) {
      fwrite(&CHUNK_VERSION, 2, 1, file);
      uint32_t id;
      for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
        id = _blocks[i].getRawValue();
        fwrite(&id, 4, 1, file);
      }

      fclose(file);
    } else {
      std::cout << "Chunk Write Error" << std::endl;
    }
  }
}

void Chunk::loadChunk(const char* dir, ChunkGenerator* gen) {
  std::lock_guard<std::mutex> lock(_blockLock);
  deleteBlocks();
  _blocks = new Block[BLOCK_COUNT];
  if (_blocks == nullptr) return;
  FILE* file = nullptr;
  std::string path =
      dir + posToString(_position / 100) + "\\" + posToString(_position) + ".ch";
  if (fopen_s(&file, path.c_str(), "rb") == 0 && file != 0) {
    unsigned short fileVersion;
    fread(&fileVersion, 2, 1, file);
    // add check for file version vs current version

    uint32_t id;
    for (unsigned int i = 0; i < BLOCK_COUNT; ++i) {
      fread(&id, 4, 1, file);
      if (id != 0) {
        _blocks[i] = Block(id);
      }
    }

    fclose(file);
    _blocksModified = false;
  } else {
    gen->genChunkBlocks(_position, _blocks);
    _blocksModified = true;
  }
}

bool Chunk::drawBlockFace(Block* block, BlockBase::Face face) {
  if (*block == 0) return true;
  return !Blocks::getBlock((int)block)->getFullBlockFace(face);
}

void Chunk::deleteBlocks() {
  if (_blocks != nullptr) {
    delete[] _blocks;
    _blocks = nullptr;
  }
}
