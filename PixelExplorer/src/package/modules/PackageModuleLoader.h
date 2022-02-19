#ifndef PACKAGEMODULELOADER_H
#define PACKAGEMODULELOADER_H
#include <cstdint>
#include <string>

#include "DummyModule.h"
#include "PackageModule.h"
#include "RefCounted.h"

class PackageModuleLoader : public virtual RefCounted {
 public:
  virtual PackageModule* Load(std::string packagePath) = 0;
  virtual ~PackageModuleLoader() {}
  uint16_t ModuleId() const { return _packageId; }
  static PackageModule* CreateDummyModule(uint16_t id) {
    return new DummyModule(id);
  }

 protected:
  PackageModuleLoader(uint16_t packageId, uint8_t priority)
      : _packageId(packageId), _loadPriority(priority) {}

 private:
  uint16_t _packageId;
  uint8_t _loadPriority;
};

#endif  // !SUBPACKAGELOADER_H
