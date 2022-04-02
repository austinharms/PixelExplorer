#ifndef BLOCKDEFINITION_H
#define BLOCKDEFINITION_H
#include <cstdint>
#include <string>
#include <unordered_map>

#include "BlockShape.h"
#include "RefCounted.h"
#include "util/Direction.h"
namespace px::game::chunk {
class BlockDefinition : public RefCounted {
 public:
  static BlockDefinition* createDefaultDefinition(BlockShape* shape);

  const std::string Name;
  BlockShape* Shape;
  const float UVOffset[util::Direction::DIRECTION_COUNT * 2];
  const int16_t Id;

  BlockDefinition(std::string name, int16_t id,
                  float uv[util::Direction::DIRECTION_COUNT * 2],
                  BlockShape* shape);
  virtual ~BlockDefinition();
};
}  // namespace px::game::chunk
#endif  // !BLOCKDEFINITION_H
