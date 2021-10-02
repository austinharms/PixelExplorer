#ifndef BLOCK_H
#define BLOCK_H
#include <cstdint>

#include "RefCounted.h"
#include "ChunkBlock.h"
#include "RawBlock.h"
#include "BlockFace.h"
#include "BlockData.h"
#include "../Chunk.h"

class Block : public virtual RefCounted {
 public:
  Block(Chunk* chunk, ChunkBlock* block = nullptr, uint32_t index = 0) {
    chunk->grab();
    _parentChunk = chunk;
    _rawBlock = nullptr;
    _chunkBlock = nullptr;
    _extendedData = nullptr;
    _extended = false;
    _checkExtended = false;
    if (block != nullptr)
      setChunkBlock(block, index);
  }

  virtual ~Block() {
    _parentChunk->drop();
  }

  void setChunkBlock(ChunkBlock* block, uint32_t index) {
    _chunkBlock = block;
    _blockIndex = index;
    if (_rawBlock != nullptr) _rawBlock->drop();
    _rawBlock = RawBlock::getBlock(block->getID());
    _rawBlock->grab();
    _extendedData = nullptr;
    _checkExtended = false;
    _extended = false;
  }

  const BlockFace* getBlockFace(FaceDirection face) const {
    return _rawBlock->getBlockFace(*_chunkBlock, face);
  }

  const bool isSolid() const { return _rawBlock->isSolid(); }

  const bool isFaceSolid(FaceDirection face) const {
    if (isSolid()) return true;
    return getBlockFace(face)->solid;
  }

  const bool isExtended() { 
    if (_checkExtended) return _extended;
    _extended = _chunkBlock->isExtended();
    _checkExtended = true;
    return _extended;
  }

  BlockData* getExtendedData() {
    if (!isExtended()) return nullptr;
    if (_extendedData != nullptr) return _extendedData;
    _parentChunk->getBlockData(_blockIndex);
  }

 private:
  Chunk* _parentChunk;
  RawBlock* _rawBlock;
  ChunkBlock* _chunkBlock;
  uint32_t _blockIndex;
  BlockData* _extendedData;
  bool _checkExtended;
  bool _extended;
};
#endif