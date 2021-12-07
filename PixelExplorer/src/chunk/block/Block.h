#ifndef BLOCK_H
#define BLOCK_H
#include <cstdint>

#include "RefCounted.h"
#include "ChunkBlock.h"
#include "BaseBlock.h"
#include "BlockFace.h"
#include "BlockData.h"
#include "../Chunk.h"

class Block : public virtual RefCounted {
 public:
  Block(Chunk* chunk, ChunkBlock* block = nullptr, uint32_t index = 0) {
    chunk->grab();
    _parentChunk = chunk;
    _baseBlock = nullptr;
    _chunkBlock = nullptr;
    _extendedData = nullptr;
    _extended = false;
    _checkExtended = false;
    _blockIndex = index;
    if (block != nullptr)
      setChunkBlock(block, index);
  }

  virtual ~Block() {
    _parentChunk->drop();
    if (_baseBlock != nullptr) _baseBlock->drop();
  }

  void setChunkBlock(ChunkBlock* block, uint32_t index) {
      _chunkBlock = block;
      _blockIndex = index;
      if (_baseBlock != nullptr) _baseBlock->drop();
      uint32_t id = block->getID();
      _empty = id == 0;
      if (!_empty) {
          _baseBlock = BaseBlock::getBlock(id);
          _baseBlock->grab();
      }
      else {
          _baseBlock = nullptr;
      }

      _extendedData = nullptr;
      _checkExtended = false;
      _extended = false;
  }

  void setChunkBlock(ChunkBlock* block, uint32_t index, Chunk* chunk) {
      if (_parentChunk != chunk) {
          _parentChunk->drop();
          chunk->grab();
          _parentChunk = chunk;
      }

      _chunkBlock = block;
      _blockIndex = index;
      if (_baseBlock != nullptr) _baseBlock->drop();
      uint32_t id = block->getID();
      _empty = id == 0;
      if (!_empty) {
          _baseBlock = BaseBlock::getBlock(id);
          _baseBlock->grab();
      }
      else {
          _baseBlock = nullptr;
      }

      _extendedData = nullptr;
      _checkExtended = false;
      _extended = false;
  }

  const uint32_t getId() const { return _chunkBlock->getID(); }

  const BlockFace* getBlockFace(FaceDirection face) const {
    return _baseBlock->getBlockFace(*_chunkBlock, face);
  }

  const bool isSolid() const { return _baseBlock->isSolid(); }

  const bool isEmpty() const { return _empty; }

  const bool isFaceFull(FaceDirection face) const {
    // Can slow down chunk mesh creation due to redundant chunk
    //if (isSolid()) return true;

    return getBlockFace(face)->full;
  }

  const bool isFaceTransparent(FaceDirection face) const {
    // Can slow down chunk mesh creation due to redundant chunk
    //if (isSolid()) return false;

    return getBlockFace(face)->transparent;
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
  BaseBlock* _baseBlock;
  ChunkBlock* _chunkBlock;
  BlockData* _extendedData;
  uint32_t _blockIndex;
  bool _checkExtended;
  bool _extended;
  bool _empty;
};
#endif