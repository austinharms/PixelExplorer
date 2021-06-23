#pragma once
#include <cstdint>
#include <mutex>
#include <unordered_map>

#include "RefCounted.h"

class Block : public virtual RefCounted {
 public:
  enum Face { TOP = 0, BOTTOM = 1, FRONT = 2, BACK = 3, LEFT = 4, RIGHT = 5 };
  struct BlockFace {
    BlockFace() {
      vertexCount = 0;
      indexCount = 0;
      vertices = nullptr;
      uvs = nullptr;
      indices = nullptr;
      fullFace = true;
    }

    ~BlockFace() {
      delete[] vertices;
      delete[] uvs;
      delete[] indices;
    }

    float* vertices;
    float* uvs;
    unsigned char* indices;
    unsigned char vertexCount;
    unsigned char indexCount;
    bool fullFace;
  };

  Block(uint32_t id, bool transparent, BlockFace* blockFaces);
  virtual ~Block();
  uint32_t getID() const { return _id; }
  static void loadBlocks(const char* blockDataPath);
  static Block* getBlock(uint32_t id);
  static bool addBlock(Block* block);
  static void saveBlocks(const char* blockDataPath);
  static void dropBlocks();

  static void setDefaultBlock(Block* block) {
    std::lock_guard<std::recursive_mutex> lock(s_blockMapLock);
    if (s_defaultBlock != nullptr) s_defaultBlock->drop();
    block->grab();
    s_defaultBlock = block;
  }

  BlockFace* getBlockFace(Face f) const { return &_faces[(int)f]; }

  bool getFullBlockFace(Face f) const { return &_faces[(int)f].fullFace; }

  bool getTransparent() const { return _transparent; }

 private:
  static std::recursive_mutex s_blockMapLock;
  static std::unordered_map<uint32_t, Block*>* s_blocks;
  static Block* s_defaultBlock;
  BlockFace* _faces;
  uint32_t _id;
  bool _transparent;
};