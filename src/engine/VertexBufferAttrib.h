#pragma once
#include "RefCounted.h"

class VertexBufferAttrib : public virtual RefCounted {
 public:
  VertexBufferAttrib(int componentCount, unsigned int datatype,
                     bool normalized = false, unsigned int index = 0,
                     void* offset = 0, unsigned short componentOffset = 0)
      : componentCount(componentCount),
        datatype(datatype),
        normalized(normalized),
        index(index),
        offset(offset),
        componentOffset(componentOffset) {}
  virtual ~VertexBufferAttrib(){};
  int componentCount;
  unsigned int datatype;
  bool normalized;
  unsigned int index;
  unsigned short componentOffset;
  void* offset;
};