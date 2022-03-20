#ifndef BLOCK_H
#define BLOCK_H
#include <cstdint>

class Block {
 public:
  inline Block() : _id(0) {}
  inline Block(uint16_t id) : _id(id) {}
  inline ~Block() {}

  inline uint16_t GetId() const { _id & ID_BITMASK; }
  inline bool IsExtended() const { return _id & EXTEND_BITMASK; }

 private:
  const uint16_t ID_BITMASK = 0b0111111111111111;
  const uint16_t EXTEND_BITMASK = 0b1000000000000000;
  uint16_t _id;
};
#endif