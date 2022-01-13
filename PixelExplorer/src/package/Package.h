#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>
#include <unordered_map>

#include "RefCounted.h"
#include "modules/PackageModule.h"
#include "modules/PackageModuleId.h"

class Package : public virtual RefCounted {
  friend class PackageLoader;

 public:
  virtual ~Package() {
    _modulesLoaded = false;
    for (std::pair<uint16_t, PackageModule*> pair : _modules)
      pair.second->drop();

    _modules.clear();
  }

  void AddModule(PackageModule* pkModule) {
    if (pkModule != nullptr) {
      pkModule->grab();
      _modules.emplace(pkModule->ModuleId(), pkModule);
    }
  }

  template <typename T>
  PackageModule* GetModule() const {
    return GetModule(PackageModuleId::Id<T>());
  }

  PackageModule* GetModule(uint16_t id) const { return nullptr; }

 protected:
  std::unordered_map<uint16_t, PackageModule*> _modules;

 private:
  Package() {}

  bool _modulesLoaded;
  bool _propertiesLoaded;
  uint8_t _packageVersion[3];
  uint8_t _minAppVersion[3];
  uint8_t _maxAppVersion[3];
};

#endif  // !PACKAGELOADER_H
