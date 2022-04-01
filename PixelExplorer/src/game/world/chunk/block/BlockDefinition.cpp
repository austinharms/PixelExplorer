#include "BlockDefinition.h"

#include "Logger.h"
namespace px::game::chunk {
BlockDefinition* BlockDefinition::createDefaultDefinition(BlockShape* shape) {
  float uvs[util::Direction::DIRECTION_COUNT * 2];
  memset(uvs, 0, sizeof(uvs));
  return new BlockDefinition("DEFAULT", 0, uvs, shape);
}

BlockDefinition::BlockDefinition(std::string name, int16_t id,
                                 float uv[util::Direction::DIRECTION_COUNT * 2],
                                 BlockShape* shape)
    : Name(name), Id(id), UVOffset() {
  Shape = shape;
  Shape->grab();
  memcpy((void*)UVOffset, uv, sizeof(UVOffset));
}

BlockDefinition::~BlockDefinition() { Shape->drop(); }
}  // namespace px::game::chunk
