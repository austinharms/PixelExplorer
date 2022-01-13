#ifndef PACKAGELOADER_H
#define PACKAGELOADER_H

#include <unordered_map>

#include "Package.h"
#include "modules/PackageModule.h"
#include "modules/PackageModuleId.h"
#include "modules/PackageModuleLoader.h"

class PackageLoader {
 public:
  static Package* LoadPackage(std::string packageDirectory);
  static void RegisterModuleLoader(PackageModuleLoader* loader) {
    loader->grab();
    s_moduleLoaders.emplace(loader->ModuleId(), loader);
  }

  static void UnregisterModuleLoaders() {
    for (std::pair<uint16_t, PackageModuleLoader*> pair : s_moduleLoaders)
      pair.second->drop();

    s_moduleLoaders.clear();
  }

  static void UnregisterModuleLoader(uint16_t id) {

  }

  template <typename T>
  static void UnregisterModuleLoader() {
    UnregisterModuleLoader(PackageModuleId::Id<T>());
  }

 private:
  static std::unordered_map<uint16_t, PackageModuleLoader*> s_moduleLoaders;

  PackageLoader() {}
  ~PackageLoader() {}
};

#endif