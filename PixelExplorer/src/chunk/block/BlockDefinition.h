#ifndef BLOCKDEFINITION_H
#define BLOCKDEFINITION_H
#include <cstdint>
#include <string>
#include <unordered_map>

#include "BlockShape.h"
#include "RefCounted.h"

class BlockDefinition : public RefCounted {
 public:
  static void LoadDefinitions();
  static void UnloadDefinitions();
  static const BlockDefinition* GetDefinitionById(uint16_t id);
  static bool GetDefinitionsLoaded();
  static const BlockDefinition* GetDefaultDefinition();

  const std::string Name;

  virtual ~BlockDefinition();
  const BlockShape* GetBaseShape() const;
  const float* GetUVOffsets() const;
  const uint16_t GetId() const;

 private:
  static std::unordered_map<uint16_t, BlockDefinition*> s_blockDefinitions;
  static BlockDefinition* s_defaultDefinition;
  static bool s_definitionsLoaded;

  static BlockDefinition* CreateDefaultDefinition();

  BlockShape* _shape;
  float _UVOffset[BlockShape::FACE_COUNT * 2];
  uint16_t _id;

  BlockDefinition(std::string name);
};
#endif  // !BLOCKDEFINITION_H
