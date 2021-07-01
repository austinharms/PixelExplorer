#pragma once
#include <cstdint>
#include <mutex>
#include <vector>

#include "BlockBase.h"

class Blocks {
 public:
  static void loadBlocks(const char* blockDataPath);
  static void addBlock(BlockBase* block, int32_t id);
  static int32_t addBlock(BlockBase* block);
  static BlockBase* getBlock(int32_t id);
  static void saveBlocks(const char* blockDataPath);
  static void dropBlocks();
  static void setDefaultBlock(BlockBase* block);

 private:
  Blocks();
  ~Blocks();

  static std::recursive_mutex s_blocksLock;
  static std::vector<BlockBase*>* s_blocksArray;
  static BlockBase* s_defaultBlock;
};