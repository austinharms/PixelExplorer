#include "DataBuffer.h"
template <typename T>
DataBuffer<T>::DataBuffer(uint32_t length) : Length(length) {
  ReadOnly = false;
  Buffer = (T*)malloc(sizeof(T) * length);
}

template <typename T>
DataBuffer<T>::~DataBuffer() {
  free(Buffer);
}

template <typename T>
uint32_t DataBuffer<T>::GetSize() const {
  return Length * sizeof(T);
}