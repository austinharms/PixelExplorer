#ifndef BLOCKSET_H
#define BLOCKSET_H

#include <string>
#include <unordered_map>
#include <stdint.h>

#include "BlockShape.h"
#include "BlockDefinition.h"
#include "RefCounted.h"
namespace px::game::chunk {
class BlockSet : public RefCounted {
 public:
  BlockSet();
  virtual ~BlockSet();
  BlockShape* getDefaultShape() const;
  BlockShape* getShape(const std::string shapeName) const;
  const BlockDefinition* getDefinitionById(const uint16_t id) const;
  const BlockDefinition* getDefaultDefinition() const;

 private:
  BlockShape* _defaultShape;
  BlockDefinition* _defaultDefinition;
  std::unordered_map<std::string, BlockShape*> _blockShapes;
  std::unordered_map<uint16_t, BlockDefinition*> _blockDefinitions;
};
}  // namespace px::game::chunk
#endif  // !BLOCKSET_H
