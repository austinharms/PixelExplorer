#include "BlockDefinition.h"
#include "Logger.h"

std::unordered_map<uint16_t, BlockDefinition*>
    BlockDefinition::s_blockDefinitions;
bool BlockDefinition::s_definitionsLoaded = false;
BlockDefinition* BlockDefinition::s_defaultDefinition = nullptr;

BlockDefinition::BlockDefinition(std::string name) : Name(name) {}

BlockDefinition* BlockDefinition::createDefaultDefinition() {
  BlockDefinition* blockDef = new BlockDefinition("DEFAULT");
  blockDef->_id = 0;
  blockDef->_shape = BlockShape::getDefaultShape();
  memset(blockDef->_UVOffset, 0, sizeof(blockDef->_UVOffset));
  Logger::debug("Created DEFAULT Block Definition");
  return blockDef;
}

void BlockDefinition::loadDefinitions() {
  if (s_definitionsLoaded) return;
  Logger::info("Loading Block Definitions");
  s_defaultDefinition = createDefaultDefinition();
  s_definitionsLoaded = true;
  Logger::info("Done Loading Block Definitions");
}

void BlockDefinition::unloadDefinitions() {
  if (!s_definitionsLoaded) return;
  Logger::info("Unloading Block Definitions");
  s_definitionsLoaded = false;
  s_defaultDefinition->drop();
  s_defaultDefinition = nullptr;
  for (auto def : s_blockDefinitions) def.second->drop();
  s_blockDefinitions.clear();
  Logger::info("Done Unloading Block Definitions");
}

const BlockDefinition* BlockDefinition::getDefinitionById(uint16_t id) {
  if (!s_definitionsLoaded) return nullptr;
  auto found = s_blockDefinitions.find(id);
  if (found == s_blockDefinitions.end()) return s_defaultDefinition;
  return found->second;
}

bool BlockDefinition::getDefinitionsLoaded() {
  return s_definitionsLoaded;
}

const BlockDefinition* BlockDefinition::getDefaultDefinition() {
  return s_defaultDefinition;
}

BlockDefinition::~BlockDefinition() {}

const BlockShape* BlockDefinition::getBaseShape() const { return _shape; }

const float* BlockDefinition::getUVOffsets() const { return _UVOffset; }

const uint16_t BlockDefinition::getId() const { return _id; }
