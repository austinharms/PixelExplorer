#ifndef SUBPACKAGE_H
#define SUBPACKAGE_H

#include <cstdint>

class SubPackage {
 public:
  virtual uint16_t PackageId() = 0;
  virtual ~SubPackage() {}

 protected:
  SubPackage() {}
};

#endif  // !SUBPACKAGE_H
