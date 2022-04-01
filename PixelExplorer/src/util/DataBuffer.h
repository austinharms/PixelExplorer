#ifndef DATABUFFER_H
#define DATABUFFER_H
#include <stdint.h>

#include "RefCounted.h"
namespace px::util {
template <typename T>
class DataBuffer : public RefCounted {
 public:
  T* buffer;
  const uint32_t length;
  bool isReadOnly();
  void makeReadOnly();
  void makeWriteable();
  DataBuffer(uint32_t length);
  ~DataBuffer();
  uint32_t getSize() const;

 private:
  uint8_t _readOnlyCounter;
};
}  // namespace px::util

#include "DataBuffer.inl"

#endif  // !DATABUFFER_H