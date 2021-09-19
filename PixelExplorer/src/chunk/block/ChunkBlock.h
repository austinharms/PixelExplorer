#ifndef CHUNKBLOCK_H
#define CHUNKBLOCK_H

#include <cstdint>

class ChunkBlock {
 public:
  ChunkBlock(uint32_t id, bool raw = false) {
    if (!raw) {
      _value = 0;
      setId(id);
    } else {
      _value = id;
    }
  }

  ChunkBlock() { _value = 0; }

  uint32_t getID() const { return _value & g_idBitmask; }

  void setId(uint32_t id) {
    _value &= ~g_idBitmask;
    _value |= (id & g_idBitmask);
  }

  unsigned char getBools() { return _value >> 24; }

  void setBools(uint8_t values) {
    _value &= ~g_boolsBitmask;
    _value |= (values << 24);
  }

  bool getBool(uint8_t index) { return false; }

  void setBool(uint8_t index, bool value) {}

  uint8_t getRotation() { return (_value & g_rotationBitmask) >> 19; }

  void setRotation(uint8_t rotation) {
    _value &= ~g_rotationBitmask;
    _value |= ((rotation << 19) & g_rotationBitmask);
  }

  uint32_t getRawValue() const { return _value; }

  ChunkBlock& operator=(const ChunkBlock& other) {
    if (this != &other) this->_value = other._value;
    return *this;
  }

  void setExtended(bool state) { 
    _value &= ~g_extendedBitmask;
    if (state) _value |= g_extendedBitmask;
  }

  bool isExtended() const { return _value & g_extendedBitmask; };

  inline friend bool operator==(const ChunkBlock& lhs, const ChunkBlock& rhs) {
    return lhs._value == rhs._value;
  }

  inline friend bool operator==(const ChunkBlock& lhs, const uint32_t& rhs) {
    return lhs.getID() == rhs;
  }

  inline friend bool operator==(const uint32_t& lhs, const ChunkBlock& rhs) {
    return rhs.getID() == lhs;
  }

  explicit operator uint32_t() const { return this->getID(); }

 private:
  static const uint32_t g_idBitmask = 0b00000000000001111111111111111111;
  static const uint32_t g_rotationBitmask = 0b00000000111110000000000000000000;
  static const uint32_t g_boolsBitmask = 0b01111111000000000000000000000000;
  static const uint32_t g_extendedBitmask = 0b10000000000000000000000000000000;
  uint32_t _value;
};

#endif  // !CHUNKBLOCK_H
