#include "BlockSet.h"
#include "Logger.h"
namespace px::game::chunk {
BlockSet::BlockSet() {
  _defaultShape = BlockShape::createDefaultShape();
  _defaultShape->grab();
  _blockShapes.emplace("DEFAULT", _defaultShape);

  _defaultDefinition = BlockDefinition::createDefaultDefinition(_defaultShape);
  _defaultDefinition->grab();
  _blockDefinitions.emplace(_defaultDefinition->Id, _defaultDefinition);
  Logger::info("Loaded Block Set");
}

BlockSet::~BlockSet() {
  for (auto it = _blockDefinitions.begin(); it != _blockDefinitions.end(); ++it)
    it->second->drop();
  _defaultDefinition->drop();

  for (auto it = _blockShapes.begin(); it != _blockShapes.end(); ++it)
    it->second->drop();
  _defaultShape->drop();
  Logger::info("Unloaded Block Set");
}

BlockShape* BlockSet::getDefaultShape() const { return _defaultShape; }

BlockShape* BlockSet::getShape(const std::string shapeName) const { 
  auto it = _blockShapes.find(shapeName);
  if (it != _blockShapes.end()) return it->second;
  return _defaultShape;
}

const BlockDefinition* BlockSet::getDefinitionById(const uint16_t id) const {
  auto it = _blockDefinitions.find(id);
  if (it != _blockDefinitions.end()) return it->second;
  return _defaultDefinition;
}

const BlockDefinition* BlockSet::getDefaultDefinition() const {
  return _defaultDefinition;
}
}  // namespace px::game::chunk