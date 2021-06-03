#include "Block.h"
#include <unordered_map>
std::unordered_map<uint32_t, Block*> Block::s_blocks =
    std::unordered_map<uint32_t, Block*>();
Block* Block::s_defaultBlock = nullptr;

Block::Block(uint32_t id, bool transparent, BlockFace* blockFaces)
    : _id(id), _transparent(transparent), _faces(blockFaces) {}

Block::~Block() { delete[] _faces; }

Block* Block::getBlock(uint32_t id) {
  auto block = Block::s_blocks.find(id);
  if (block != Block::s_blocks.end()) {
    return block->second;
  } else {
    return s_defaultBlock;
  }
}

bool Block::addBlock(Block* block) {
  return Block::s_blocks.insert({block->_id, block}).second;
}

void Block::dropBlocks() {
  if (s_defaultBlock != nullptr) s_defaultBlock->drop();
  for (const std::pair<uint32_t, Block*>& block : Block::s_blocks) {
    block.second->drop();
  }
}
