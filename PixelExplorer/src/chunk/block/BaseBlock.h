#ifndef RAWBLOCK_H
#define RAWBLOCK_H
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "../FaceDirection.h"
#include "BlockFace.h"
#include "ChunkBlock.h"
#include "RefCounted.h"
#include "World.h"

class BaseBlock : public virtual RefCounted {
 public:
  const static uint16_t VERSION = 1;
  virtual ~BaseBlock();
  static void LoadBlockManifest();
  static bool LoadPackage(std::string name, bool updateManifest = true);

  const BlockFace* getBlockFace(FaceDirection dir) const {
    assert(dir != FaceDirection::NONE);
    return (const BlockFace*)&_faces[(uint8_t)dir - 1];
  }

  const BlockFace* getBlockFace(ChunkBlock& block, FaceDirection dir) const {
    // do block rotation math here
    assert(dir != FaceDirection::NONE);
    return (const BlockFace*)&_faces[(uint8_t)dir - 1];
  }

  bool isSolid() const { return _solid; }

  uint32_t getId() const { return _id; }

  const char* getName() const { return (const char*)_name; }

  static BaseBlock* getBlock(uint32_t id) {
    if (id >= s_blockCount) return nullptr;
    return &s_blocks[id];
  }

 private:
  BaseBlock();
  static void UpdateManifest();

  static BaseBlock* s_blocks;
  static uint32_t s_blockCount;
  static std::vector<std::string> s_packages;
  static std::unordered_map<std::string, uint32_t> s_blockLookupTable;
  uint32_t _id;
  bool _solid;
  bool _loaded;
  BlockFace _faces[6];
  const char* _name;
};
#endif