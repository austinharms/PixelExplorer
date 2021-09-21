#ifndef BLOCK_H
#define BLOCK_H
#include <cstdint>

#include "RefCounted.h"
#include "ChunkBlock.h"
#include "RawBlock.h"
#include "BlockFace.h"
#include "../Chunk.h"

class Block : public virtual RefCounted {
 public:
  Block(Chunk* chunk, ChunkBlock* block = nullptr) {
    chunk->grab();
    _parentChunk = chunk;
    _rawBlock = nullptr;
    _chunkBlock = nullptr;
    _extendedData = nullptr;
    _extended = false;
    _checkExtended = false;
    if (block != nullptr)
      setChunkBlock(block);
  }

  virtual ~Block() {
    _parentChunk->drop();
  }

  void setChunkBlock(ChunkBlock* block) {
    _chunkBlock = block;
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

  const uint8_t* getExtendedData() {
    if (!isExtended()) return nullptr;
    if (_extendedData != nullptr) return _extendedData;
    _parentChunk->get
  }

 private:
  Chunk* _parentChunk;
  RawBlock* _rawBlock;
  ChunkBlock* _chunkBlock;
  uint8_t* _extendedData;
  bool _checkExtended;
  bool _extended;
};
#endif