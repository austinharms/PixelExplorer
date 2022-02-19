#ifndef RAWBLOCK_H
#define RAWBLOCK_H
#include <cstdint>
#include <fstream>
#include <set>
#include <string>
#include <unordered_map>

#include "../FaceDirection.h"
#include "BlockFace.h"
#include "ChunkBlock.h"
#include "RefCounted.h"
#include "World.h"
#include "rendering/TexturedMaterial.h"

class BaseBlock {  // : public virtual RefCounted {
  friend class BlockLoader;
 public:
  const static uint16_t MANIFEST_VERSION = 1;
  virtual ~BaseBlock();
  static void StartBlockLoading();
  static void FinalizeBlockLoading();
  static void RegisterBlocks(const std::string* blockNames, uint8_t blockCount);
  static void LoadBlock(BaseBlock& block);
  static void UnloadBlocks();
  static uint32_t GetBlockID(const std::string blockName);
  static BaseBlock* GetBlock(uint32_t id) {
    if (id >= s_blockCount) return &s_blocks[0];
    return &s_blocks[id];
  }

  const BlockFace* getBlockFace(FaceDirection dir) const {
    assert(dir != FaceDirection::NONE);
    return (const BlockFace*)&_faces[(uint8_t)dir];
  }

  const BlockFace* getBlockFace(ChunkBlock& block, FaceDirection dir) const {
    // do block rotation math here
    assert(dir != FaceDirection::NONE);
    return (const BlockFace*)&_faces[(uint8_t)dir];
  }

  bool isSolid() const { return _solid; }

  uint32_t getId() const { return _id; }

  const std::string getName() const { return _name; }

 private:
  static void SaveManifest();
  static void CreateTextureAtlas();

  static BaseBlock* s_blocks;
  static uint32_t s_blockCount;
  static std::unordered_map<std::string, uint32_t> s_blockLookupTable;
  static bool s_loadingBlocks;

  BaseBlock(const BaseBlock& block);
  BaseBlock();
  BaseBlock& operator=(const BaseBlock& block);

  uint32_t _id;
  bool _solid;
  bool _loaded = 0;
  BlockFace _faces[(uint32_t)FaceDirection::FACECOUNT];
  std::string _name;
};
#endif