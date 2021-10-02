#ifndef BLOCKDATA_H
#define BLOCKDATA_H
#include <cstdint>

#include "RefCounted.h"

class BlockData : public virtual RefCounted {
 public:
  BlockData() : _data(nullptr), _byteCount(0) {}
  virtual ~BlockData() {}

 private:
  uint8_t* _data;
  uint32_t _byteCount;
};
#endif