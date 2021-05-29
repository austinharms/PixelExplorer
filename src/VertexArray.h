#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H
#include <list>
#include <GL/glew.h>

#include "RefCounted.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexBufferAttrib.h"

class VertexArray : public virtual RefCounted {
 public:
  VertexArray();
  virtual ~VertexArray();
  void bind() const;
  void unbind() const;
  unsigned int addVertexBuffer(VertexBuffer* buffer,
                               VertexBufferAttrib* layout[],
                       short layoutAttribCount);
  // void updateGLArray() const;
  unsigned int getGLID() const { return _renderId; }

 private:

  // mutable bool _dirtyBuffer;
  unsigned int _renderId;
  unsigned int _nextBufferId;
  unsigned int _nextAttribIndex;
  std::list<VertexBufferLayout*>* _buffers;
  void genGLArray();
};

/*
unsigned int vertArray;
  glGenVertexArrays(1, &vertArray);
  glBindVertexArray(vertArray);

  unsigned int vertBuff;
  glGenBuffers(1, &vertBuff);
  glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
  glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), block, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5,
                        (const void*)(sizeof(float) * 3));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
*/
#endif