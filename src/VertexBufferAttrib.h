#ifndef VERTEXBUFFERATTRIB_H
#define VERTEXBUFFERATTRIB_H
#include "RefCounted.h"

class VertexBufferAttrib : public virtual RefCounted {
 public:
  VertexBufferAttrib(int componentCount, unsigned int datatype,
                     bool normalized = false, unsigned int index = 0,
                     void* offset = 0)
      : componentCount(componentCount),
        datatype(datatype),
        normalized(normalized),
        index(index),
        offset(offset) {}
  virtual ~VertexBufferAttrib(){};
  int componentCount;
  unsigned int datatype;
  bool normalized;
  unsigned int index;
  void* offset;
};

#endif