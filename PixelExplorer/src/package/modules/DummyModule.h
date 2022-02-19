#ifndef DUMMYMODULE_H

#include "PackageModule.h"
class DummyModule : public PackageModule {
  friend class PackageModuleLoader;

 public:
  virtual ~DummyModule() {}

 private:
  DummyModule(uint16_t id) : PackageModule(id) {}
};
#endif  // !DUMMYMODULE_H
