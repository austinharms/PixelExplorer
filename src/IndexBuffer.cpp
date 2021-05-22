#include "IndexBuffer.h"

#include "RefCounted.h"
#include <GL/glew.h>

IndexBuffer::IndexBuffer()
    : _size(0), _length(0), _buffer(nullptr), _renderId(0) {
  genGLBuffer();
}

IndexBuffer::IndexBuffer(unsigned char size, unsigned int length, void* data)
    : _size(size), _length(length), _buffer(data), _renderId(0) {
  genGLBuffer();
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &_renderId); }

void IndexBuffer::bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _renderId);
}

void IndexBuffer::unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::updateGLBuffer() {
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size * _length, _buffer,
               GL_STATIC_DRAW);
}

void IndexBuffer::genGLBuffer() { 
  glGenBuffers(1, &_renderId);
  bind();
  updateGLBuffer();
  unbind();
}


/*
  unsigned int indexBuff;
  glGenBuffers(1, &indexBuff);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), index,
               GL_STATIC_DRAW);
               */