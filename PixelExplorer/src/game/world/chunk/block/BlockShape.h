#ifndef BLOCKSHAPE_H
#define BLOCKSHAPE_H
#include <cstdint>
#include <string>
#include <unordered_map>

#include "RefCounted.h"
#include "util/Direction.h"
namespace px::game::chunk {
struct BlockShape : public RefCounted {
 public:
  static BlockShape* createDefaultShape();

  bool FullBlock;
  bool BlockTransparent;
  bool HasPhysicsShape;
  bool HasRenderShape;
  float* RenderVertices[util::Direction::DIRECTION_COUNT];
  float* PhysicsVertices[util::Direction::DIRECTION_COUNT];
  uint8_t* RenderIndices[util::Direction::DIRECTION_COUNT];
  uint8_t* PhysicsIndices[util::Direction::DIRECTION_COUNT];
  uint8_t RenderIndexCount[util::Direction::DIRECTION_COUNT];
  uint8_t PhysicsIndexCount[util::Direction::DIRECTION_COUNT];
  uint8_t RenderVertexCount[util::Direction::DIRECTION_COUNT];
  uint8_t PhysicsVertexCount[util::Direction::DIRECTION_COUNT];
  bool TransparentFace[util::Direction::DIRECTION_COUNT];
  const std::string Name;

  BlockShape(std::string name);
  virtual ~BlockShape();
};
}  // namespace px::game::chunk
#endif