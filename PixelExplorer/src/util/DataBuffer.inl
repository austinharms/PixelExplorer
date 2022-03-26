#include "DataBuffer.h"
namespace px::util {
template <typename T>
bool DataBuffer<T>::isReadOnly() {
  return _readOnlyCounter > 0;
}

template <typename T>
void DataBuffer<T>::makeReadOnly() {
  ++_readOnlyCounter;
}

template <typename T>
void DataBuffer<T>::makeWriteable() {
  --_readOnlyCounter;
}

template <typename T>
DataBuffer<T>::DataBuffer(uint32_t length) : length(length) {
  _readOnlyCounter = 0;
  buffer = (T*)malloc(sizeof(T) * length);
}

template <typename T>
DataBuffer<T>::~DataBuffer() {
  free(buffer);
}

template <typename T>
uint32_t DataBuffer<T>::getSize() const {
  return length * sizeof(T);
}
}  // namespace px::util