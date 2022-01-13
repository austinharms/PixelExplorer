#ifndef PACKAGEMODULELOADER_H
#define PACKAGEMODULELOADER_H
#include <cstdint>
#include <string>

#include "RefCounted.h"
#include "PackageModule.h"

class PackageModuleLoader : public virtual RefCounted {
 public:
  virtual PackageModule* Load(std::string packagePath) = 0;
  virtual ~PackageModuleLoader() {}

  uint16_t ModuleId() const { return _packageId; }

 protected:
  PackageModuleLoader(uint16_t packageId) : _packageId(packageId) {}

 private:
  uint16_t _packageId;
};

#endif  // !SUBPACKAGELOADER_H
