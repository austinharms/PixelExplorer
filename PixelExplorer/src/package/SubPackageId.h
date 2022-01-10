#ifndef SUBPACKAGEID_H
#define SUBPACKAGEID_H

#include <cstdint>

class SubPackageId {
 public:
  template <typename T>
  static size_t Id() {
    static size_t id = s_counter++;
    return id;
  }

 private:
  static uint16_t s_counter;
  SubPackageId() {}
  ~SubPackageId() {}
};

uint16_t SubPackageId::s_counter = 0;

#endif  // !SUBPACKAGEID_H
