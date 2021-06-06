#pragma once
#include <GL/glew.h>

#include <list>

#include "RefCounted.h"
#include "VertexBuffer.h"
#include "VertexBufferAttrib.h"

class VertexBufferLayout : public virtual RefCounted {
 public:
  VertexBufferLayout(unsigned int id, unsigned int& attribIndex,
                     VertexBuffer* buffer, VertexBufferAttrib* layout[],
                     short layoutAttribCount)
      : id(id), stride(0), buffer(buffer), layout(nullptr), componentStride(0) {
    buffer->grab();
    this->layout = new std::list<VertexBufferAttrib*>();
    unsigned int size;
    for (short i = 0; i < layoutAttribCount; i++) {
      layout[i]->grab();
      switch (layout[i]->datatype) {
        case GL_BYTE:
          size = sizeof(GLbyte);
          break;
        case GL_UNSIGNED_BYTE:
          size = sizeof(GLubyte);
          break;
        case GL_SHORT:
          size = sizeof(GLshort);
          break;
        case GL_UNSIGNED_SHORT:
          size = sizeof(GLushort);
          break;
        case GL_INT:
          size = sizeof(GLint);
          break;
        case GL_UNSIGNED_INT:
          size = sizeof(GLuint);
          break;
        case GL_FIXED:
          size = sizeof(GLfixed);
          break;
        case GL_HALF_FLOAT:
          size = sizeof(GLhalf);
          break;
        case GL_FLOAT:
          size = sizeof(GLfloat);
          break;
        case GL_DOUBLE:
          size = sizeof(GLdouble);
          break;
        default:
          size = 0;
          break;
      }
      layout[i]->componentOffset = componentStride;
      layout[i]->index = attribIndex++;
      layout[i]->offset = (void*)stride;
      stride += layout[i]->componentCount * size;
      componentStride += layout[i]->componentCount;
      this->layout->push_back(layout[i]);
    }
  }
  virtual ~VertexBufferLayout() {
    for (VertexBufferAttrib* attrib : *layout) attrib->drop();
    delete layout;
    buffer->drop();
  }
  unsigned int id;
  VertexBuffer* buffer;
  int stride;
  int componentStride;
  std::list<VertexBufferAttrib*>* layout;
  unsigned short getAttribComponentOffset(unsigned short attribIndex) const {
    auto layoutI = layout->begin();
    for (unsigned short i = 0; i < attribIndex; ++i) layoutI++;
    return (*layoutI)->componentOffset;
  }

  int getStride() const { return stride; }
  int getComponentStride() const { return componentStride; }
};
