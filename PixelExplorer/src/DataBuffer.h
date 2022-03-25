#ifndef DATABUFFER_H
#define DATABUFFER_H
#include <stdint.h>

#include "RefCounted.h"

template <typename T>
class DataBuffer : public RefCounted {
 public:
  T* Buffer;
  const uint32_t Length;
  bool IsReadOnly();
  void MakeReadOnly();
  void MakeWriteable();
  DataBuffer(uint32_t length);
  ~DataBuffer();
  uint32_t GetSize() const;

 private:
  uint8_t _readOnlyCounter;
};

#include "DataBuffer.inl"

#endif  // !DATABUFFER_H