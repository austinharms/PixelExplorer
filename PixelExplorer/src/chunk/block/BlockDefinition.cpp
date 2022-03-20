#include "BlockDefinition.h"

std::unordered_map<uint16_t, BlockDefinition*> BlockDefinition::s_blockDefinitions;
bool BlockDefinition::s_definitionsLoaded = false;
BlockDefinition* BlockDefinition::s_defaultDefinition = nullptr;

BlockDefinition::BlockDefinition(std::string name) : Name(name) {}

BlockDefinition* BlockDefinition::CreateDefaultDefinition() {
  BlockDefinition* blockDef = new BlockDefinition("DEFAULT");
  blockDef->_id = 0;
  blockDef->_shape = BlockShape::GetDefaultShape();
  memset(blockDef->_UVOffset, 0, sizeof(blockDef->_UVOffset));
  return blockDef;
}

void BlockDefinition::LoadDefinitions() {
  if (s_definitionsLoaded) return;
  BlockShape::LoadShapes();
  s_defaultDefinition = CreateDefaultDefinition();
  s_definitionsLoaded = true;
}

void BlockDefinition::UnloadDefinitions() {
  if (!s_definitionsLoaded) return;
  s_definitionsLoaded = false;
  s_defaultDefinition->drop();
  s_defaultDefinition = nullptr;
  for (auto def : s_blockDefinitions) def.second->drop();
  s_blockDefinitions.clear();
  BlockShape::UnloadShapes();
}

const BlockDefinition* BlockDefinition::GetDefinitionById(uint16_t id) {
  if (!s_definitionsLoaded) return nullptr;
  auto found = s_blockDefinitions.find(id);
  if (found == s_blockDefinitions.end()) return s_defaultDefinition;
  return found->second;
}

bool BlockDefinition::GetDefinitionsLoaded() { return s_definitionsLoaded; }

const BlockDefinition* BlockDefinition::GetDefaultDefinition() {
  return s_defaultDefinition;
}

BlockDefinition::~BlockDefinition() {}
