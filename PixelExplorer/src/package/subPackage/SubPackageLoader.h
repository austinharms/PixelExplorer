#ifndef SUBPACKAGELOADER_H
#define SUBPACKAGELOADER_H
#include <string>
#include <cstdint>

#include "SubPackage.h"
#include "RefCounted.h"

class SubPackageLoader : public virtual RefCounted {
 public:
  virtual SubPackage* Load(std::string packagePath) = 0;
  virtual uint16_t PackageId() = 0;
  virtual ~SubPackageLoader() {}

protected:
  SubPackageLoader() {}
};

#endif  // !SUBPACKAGELOADER_H
