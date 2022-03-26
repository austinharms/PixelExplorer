#ifndef BLOCKDEFINITION_H
#define BLOCKDEFINITION_H
#include <cstdint>
#include <string>
#include <unordered_map>

#include "BlockShape.h"
#include "RefCounted.h"

class BlockDefinition : public RefCounted {
 public:
  static void loadDefinitions();
  static void unloadDefinitions();
  static const BlockDefinition* getDefinitionById(uint16_t id);
  static bool getDefinitionsLoaded();
  static const BlockDefinition* getDefaultDefinition();

  const std::string Name;

  virtual ~BlockDefinition();
  const BlockShape* getBaseShape() const;
  const float* getUVOffsets() const;
  const uint16_t getId() const;

 private:
  static std::unordered_map<uint16_t, BlockDefinition*> s_blockDefinitions;
  static BlockDefinition* s_defaultDefinition;
  static bool s_definitionsLoaded;

  static BlockDefinition* createDefaultDefinition();

  BlockShape* _shape;
  float _UVOffset[BlockShape::FACE_COUNT * 2];
  uint16_t _id;

  BlockDefinition(std::string name);
};
#endif  // !BLOCKDEFINITION_H
