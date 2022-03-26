#include "Block.h"
namespace px::game::chunk {
Block::Block() : _id(0) {}

Block::Block(uint16_t id) : _id(id) {}

Block::~Block() {}

uint16_t Block::getId() const { return _id & ID_BITMASK; }

bool Block::isExtended() const { return _id & EXTEND_BITMASK; }
}  // namespace px::game::chunk