#ifndef BLOCKSHAPE_H
#define BLOCKSHAPE_H
#include <cstdint>
#include <string>
#include <unordered_map>

#include "RefCounted.h"
namespace px::game::chunk {
struct BlockShape : public RefCounted {
 public:
  static const int32_t FACE_COUNT = 6;

  static bool getShapesLoaded();
  static void loadShapes();
  static BlockShape* getDefaultShape();
  static BlockShape* getShape(std::string shapeName);
  static void unloadShapes();

  bool FullBlock;
  bool BlockTransparent;
  bool HasPhysicsShape;
  bool HasRenderShape;
  float* RenderVertices[FACE_COUNT];
  float* PhysicsVertices[FACE_COUNT];
  uint8_t* RenderIndices[FACE_COUNT];
  uint8_t* PhysicsIndices[FACE_COUNT];
  uint8_t RenderIndexCount[FACE_COUNT];
  uint8_t PhysicsIndexCount[FACE_COUNT];
  uint8_t RenderVertexCount[FACE_COUNT];
  uint8_t PhysicsVertexCount[FACE_COUNT];
  bool TransparentFace[FACE_COUNT];
  const std::string Name;

  virtual ~BlockShape();

 private:
  static BlockShape* s_defaultShape;
  static std::unordered_map<std::string, BlockShape*> s_blockShapes;
  static bool s_shapesLoaded;

  static BlockShape* createDefaultShape();

  BlockShape(std::string name);
};
}  // namespace px::game::chunk
#endif