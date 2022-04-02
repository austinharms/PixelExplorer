#ifndef BLOCK_H
#define BLOCK_H
#include <cstdint>
namespace px::game::chunk {
class Block {
 public:
  Block();
  Block(uint16_t id);
  ~Block();

  uint16_t getId() const;
  bool isExtended() const;

 private:
  const static uint16_t ID_BITMASK = 0b0111111111111111;
  const static uint16_t EXTEND_BITMASK = 0b1000000000000000;
  uint16_t _id;
};
}  // namespace px::game::chunk
#endif