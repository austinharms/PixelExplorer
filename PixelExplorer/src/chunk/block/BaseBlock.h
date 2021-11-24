#ifndef RAWBLOCK_H
#define RAWBLOCK_H
#include <cstdint>
#include <fstream>
#include <string>
#include <set>
#include <unordered_map>

#include "../FaceDirection.h"
#include "BlockFace.h"
#include "ChunkBlock.h"
#include "RefCounted.h"
#include "World.h"
#include "rendering/TexturedMaterial.h"

class BaseBlock : public virtual RefCounted {
 public:
  const static uint16_t MANIFEST_VERSION = 1;
  const static uint16_t PACKAGE_VERSION = 1;
  virtual ~BaseBlock();
  static void LoadBlockManifest();
  static void UnloadBlocks();

  const BlockFace* getBlockFace(FaceDirection dir) const {
    assert(dir != FaceDirection::NONE);
    return (const BlockFace*)&_faces[(uint8_t)dir - 1];
  }

  const BlockFace* getBlockFace(ChunkBlock& block, FaceDirection dir) const {
    // do block rotation math here
    assert(dir != FaceDirection::NONE);
    return (const BlockFace*)&_faces[(uint8_t)dir];
  }

  bool isSolid() const { return _solid; }

  uint32_t getId() const { return _id; }

  const std::string getName() const { return _name; }

  static BaseBlock* getBlock(uint32_t id) {
    if (id >= s_blockCount) return &s_blocks[0];
    return &s_blocks[id];
  }

  static TexturedMaterial* GetMaterial() { return s_chunkMaterial; }

 private:
  BaseBlock();
  static void UpdateManifest();
  static bool LoadPackage(std::string name, std::set<std::string>* loadedPackages = nullptr);

  static BaseBlock* s_blocks;
  static uint32_t s_blockCount;
  static std::unordered_map<std::string, uint32_t> s_blockLookupTable;
  static TexturedMaterial* s_chunkMaterial;
  uint32_t _id;
  bool _solid;
  bool _loaded;
  BlockFace _faces[6];
  std::string _name;
};
#endif