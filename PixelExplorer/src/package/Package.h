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

  uint8_t GetLoadPriority() const { return _loadPriority; }

  const std::string GetPath() const { return _path; }

  const std::string GetName() const { return _name; }

  bool GetLoaded() const { return _propertiesLoaded && _modulesLoaded; }

 protected:
  std::unordered_map<uint16_t, PackageModule*> _modules;

 private:
  Package()
      : _name(""),
        _description(""),
        _path(""),
        _modulesLoaded(false),
        _propertiesLoaded(false),
        _loadPriority(0) {
    memset(_packageVersion, 0, sizeof(uint8_t) * 3);
    memset(_minAppVersion, 0, sizeof(uint8_t) * 3);
    memset(_maxAppVersion, 0, sizeof(uint8_t) * 3);
  }

  std::string _name;
  std::string _description;
  std::string _path;
  bool _modulesLoaded;
  bool _propertiesLoaded;
  uint8_t _loadPriority;
  uint8_t _packageVersion[3];
  uint8_t _minAppVersion[3];
  uint8_t _maxAppVersion[3];
};

#endif  // !PACKAGELOADER_H
