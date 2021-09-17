#ifndef RAWBLOCK_H
#define RAWBLOCK_H
#include <cstdint>

#include "RefCounted.h"
#include "BlockFace.h"
#include "Block.h"

class RawBlock : public virtual RefCounted {
 public:
  RawBlock();
  virtual ~RawBlock();

  const BlockFace* getBlockFace(FaceDirection dir) const {
    assert(dir != FaceDirection::NONE);
    return (const BlockFace*)&_faces[(uint8_t)dir - 1];
  }

  const BlockFace* getBlockFace(Block& block) const { 
    //do block rotation math here
    return (const BlockFace*)&_faces[0];
  }

  bool isSolid() const { return _solid; }

  uint32_t getId() const { return _id; }

 private:
  static RawBlock** s_blocks;
  static uint32_t s_blockCount;
  BlockFace _faces[6];
  uint32_t _id;
  bool _solid;
};
#endif