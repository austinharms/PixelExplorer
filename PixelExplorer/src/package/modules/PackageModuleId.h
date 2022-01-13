#ifndef PACKAGEMODULEID_H
#define PACKAGEMODULEID_H

#include <cstdint>

class PackageModuleId {
 public:
  template <typename T>
  static uint16_t Id() {
    static uint16_t id = s_counter++;
    return id;
  }

 private:
  static uint16_t s_counter;
  PackageModuleId() {}
  ~PackageModuleId() {}
};

uint16_t PackageModuleId::s_counter = 0;

#endif  // !SUBPACKAGEID_H
