#ifndef PACKAGEMODULE_H
#define PACKAGEMODULE_H

#include <cstdint>

#include "RefCounted.h"

class PackageModule : public virtual RefCounted {
 public:
  virtual ~PackageModule() {}

  uint16_t ModuleId() const { return _id; }

 protected:
  PackageModule(uint16_t id) : _id(id) {}

 private:
  uint16_t _id;
};

#endif  // !SUBPACKAGE_H
