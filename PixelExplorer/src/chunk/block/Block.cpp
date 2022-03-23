#include "Block.h"

inline Block::Block() : _id(0) {}

inline Block::Block(uint16_t id) : _id(id) {}

inline Block::~Block() {}

inline uint16_t Block::GetId() const { _id& ID_BITMASK; }

inline bool Block::IsExtended() const { return _id & EXTEND_BITMASK; }