#include "DataBuffer.h"
template <typename T>
bool DataBuffer<T>::IsReadOnly() {
  return _readOnlyCounter > 0;
}

template <typename T>
void DataBuffer<T>::MakeReadOnly() {
  ++_readOnlyCounter;
}

template <typename T>
void DataBuffer<T>::MakeWriteable() {
  --_readOnlyCounter;
}

template <typename T>
DataBuffer<T>::DataBuffer(uint32_t length) : Length(length) {
  _readOnlyCounter = 0;
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