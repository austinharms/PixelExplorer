#pragma once
#include <cstdint>
#include <unordered_map>

#include "RefCounted.h"

class Block : public virtual RefCounted {
 public:
  enum Face { TOP = 0, BOTTOM = 1, FRONT = 2, BACK = 3, LEFT = 4, RIGHT = 5 };
  struct BlockFace {
    BlockFace() { vertexCount = 0;
      indexCount = 0;
      vertices = nullptr;
      uvs = nullptr;
      indices = nullptr;
    }

    ~BlockFace() {
      delete[] vertices;
      delete[] uvs;
      delete[] indices;
    }

    unsigned char vertexCount;
    unsigned char indexCount;
    float* vertices;
    float* uvs;
    short* indices;
  };
  Block(uint32_t id, bool transparent, BlockFace* blockFaces);
  virtual ~Block();
  uint32_t getID() const { return _id; }
  BlockFace* getBlockFace(Face f) const { return &_faces[(int)f]; }
  bool getTransparent() const { return _transparent; }
  static Block* getBlock(uint32_t id);
  static bool addBlock(Block* block);
  static void dropBlocks();
  static void setDefaultBlock(Block* block) {
    if (s_defaultBlock != nullptr) s_defaultBlock->drop();
    block->grab();
    s_defaultBlock = block;
  }

 private:
  static std::unordered_map<uint32_t, Block*>* s_blocks;
  static Block* s_defaultBlock;
  uint32_t _id;
  bool _transparent;
  BlockFace* _faces;
};