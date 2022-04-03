#include "BlockSet.h"
#include "Logger.h"
namespace px::game::chunk {
	BlockSet::BlockSet() {
		_defaultShape = BlockShape::createDefaultShape();
		_defaultShape->grab();
		_blockShapes.emplace("DEFAULT", _defaultShape);

		_blockDefinitions = new BlockDefinition * [1];
		_blockDefinitions[0] = BlockDefinition::createDefaultDefinition(_defaultShape);
		_blockDefinitionCount = 1;
		Logger::info("Loaded Block Set");
	}

	BlockSet::~BlockSet() {
		for (uint16_t i = 0; i < _blockDefinitionCount; ++i)
			_blockDefinitions[i]->drop();
		delete[] _blockDefinitions;

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
		if (id >= _blockDefinitionCount) return _blockDefinitions[0];
		return _blockDefinitions[id];
	}

	const BlockDefinition* BlockSet::getDefaultDefinition() const {
		return _blockDefinitions[0];
	}
}  // namespace px::game::chunk