#include "BlockDefinition.h"
#include "Logger.h"

std::unordered_map<uint16_t, BlockDefinition*>
    BlockDefinition::s_blockDefinitions;
bool BlockDefinition::s_definitionsLoaded = false;
BlockDefinition* BlockDefinition::s_defaultDefinition = nullptr;

inline BlockDefinition::BlockDefinition(std::string name) : Name(name) {}

BlockDefinition* BlockDefinition::CreateDefaultDefinition() {
  BlockDefinition* blockDef = new BlockDefinition("DEFAULT");
  blockDef->_id = 0;
  blockDef->_shape = BlockShape::GetDefaultShape();
  memset(blockDef->_UVOffset, 0, sizeof(blockDef->_UVOffset));
  Logger::Debug("Created DEFAULT Block Definition");
  return blockDef;
}

void BlockDefinition::LoadDefinitions() {
  if (s_definitionsLoaded) return;
  BlockShape::LoadShapes();
  Logger::Info("Loading Block Definitions");
  s_defaultDefinition = CreateDefaultDefinition();
  s_definitionsLoaded = true;
  Logger::Info("Done Loading Block Definitions");
}

void BlockDefinition::UnloadDefinitions() {
  if (!s_definitionsLoaded) return;
  Logger::Info("Unloading Block Definitions");
  s_definitionsLoaded = false;
  s_defaultDefinition->drop();
  s_defaultDefinition = nullptr;
  for (auto def : s_blockDefinitions) def.second->drop();
  s_blockDefinitions.clear();
  Logger::Info("Done Unloading Block Definitions");
  BlockShape::UnloadShapes();
}

const BlockDefinition* BlockDefinition::GetDefinitionById(uint16_t id) {
  if (!s_definitionsLoaded) return nullptr;
  auto found = s_blockDefinitions.find(id);
  if (found == s_blockDefinitions.end()) return s_defaultDefinition;
  return found->second;
}

inline bool BlockDefinition::GetDefinitionsLoaded() {
  return s_definitionsLoaded;
}

inline const BlockDefinition* BlockDefinition::GetDefaultDefinition() {
  return s_defaultDefinition;
}

inline BlockDefinition::~BlockDefinition() {}

inline const BlockShape* BlockDefinition::GetBaseShape() const { return _shape; }

inline const float* BlockDefinition::GetUVOffsets() const { return _UVOffset; }

inline const uint16_t BlockDefinition::GetId() const { return _id; }
