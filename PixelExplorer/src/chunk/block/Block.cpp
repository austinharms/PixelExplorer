#include "Block.h"

Block::Block() : _id(0) {}

Block::Block(uint16_t id) : _id(id) {}

Block::~Block() {}

uint16_t Block::GetId() const { return _id & ID_BITMASK; }

bool Block::IsExtended() const { return _id & EXTEND_BITMASK; }