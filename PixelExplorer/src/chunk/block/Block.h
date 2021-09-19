#ifndef BLOCK_H
#define BLOCK_H
#include <cstdint>

#include "RefCounted.h"
#include "ChunkBlock.h"
#include "RawBlock.h"
#include "../Chunk.h"

class Block : public virtual RefCounted {
 public:
  Block(Chunk* chunk, ChunkBlock* block = nullptr) {
    chunk->grab();
    _parentChunk = chunk;
    _rawBlock = nullptr;
    _chunkBlock = nullptr;
    _extendedData = nullptr;
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
  }

 private:
  Chunk* _parentChunk;
  RawBlock* _rawBlock;
  ChunkBlock* _chunkBlock;
  uint8_t* _extendedData;
};
#endif