#pragma once
#include <cstdint>

#include "BlockBase.h"

class Block {
 public:
  Block(uint32_t id) : _id(id) {
    _baseBlock = BlockBase::getBlock(id);
   // _baseBlock->grab();
  }

 // ~Block() { _baseBlock->drop(); }

  BlockBase::BlockFace* getBlockFace(BlockBase::Face f) const {
    return _baseBlock->getBlockFace(f);
  }

  bool getFullBlockFace(BlockBase::Face f) const {
    return _baseBlock->getFullBlockFace(f);
  }

  uint32_t getID() const { return _id; }

  BlockBase* getBaseBlock() const { return _baseBlock; }

 private:
  BlockBase* _baseBlock;
  uint32_t _id;
};