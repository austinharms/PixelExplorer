#ifndef DATABUFFER_H
#define DATABUFFER_H

#include "RefCounted.h"

template <typename T>
class DataBuffer : public RefCounted {
 public:
  T* Buffer;
  const uint32_t Length;
  bool ReadOnly;

  DataBuffer(uint32_t length);
  ~DataBuffer();
  uint32_t GetSize() const;
};

#endif  // !DATABUFFER_H
