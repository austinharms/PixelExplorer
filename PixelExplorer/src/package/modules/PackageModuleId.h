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
#endif  // !SUBPACKAGEID_H
