#ifndef BLOCKMODULELOADER_H
#define BLOCKMODULELOADER_H
#include "PackageModuleId.h"
#include "PackageModuleLoader.h"
#include "PackageModule.h"
#include "package/PackageLoader.h"
#include <string>

class BlockLoader : public PackageModuleLoader {
 public:
  PackageModule* Load(std::string packagePath) { return nullptr; }
  virtual ~BlockLoader() {}

 private:
  static BlockLoader* s_instance;
  BlockLoader() : PackageModuleLoader(PackageModuleId::Id<BlockLoader>()) {
    PackageLoader::RegisterModuleLoader(s_instance);
    s_instance->drop();
    s_instance = nullptr;
  }
};

BlockLoader* BlockLoader::s_instance = new BlockLoader();
#endif  // !BLOCKMODULELOADER_H
