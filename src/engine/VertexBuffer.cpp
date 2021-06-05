#include "VertexBuffer.h"

#include <GL/glew.h>

VertexBuffer::VertexBuffer()
    : _renderId(0), _dirtyBuffer(false), _size(0), _buffer(nullptr) {
  genGLBuffer();
}

VertexBuffer::VertexBuffer(unsigned char size, unsigned int length, void* data)
    : _renderId(0), _dirtyBuffer(false), _size(0), _buffer(nullptr) {
  genGLBuffer();
  _size = size;
  _length = length;
  _buffer = data;
  _dirtyBuffer = true;
}

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &_renderId); }

void VertexBuffer::bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, _renderId);
  if (_dirtyBuffer) updateGLBuffer();
}

void VertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VertexBuffer::updateVertices(unsigned char size, unsigned int length, void* data) {
  _size = size;
  _length = length;
  _buffer = data;
  _dirtyBuffer = true;
}

void VertexBuffer::updateDirtyBuffer() {
  if (_dirtyBuffer) {
    bind();
    unbind();
  }
}

void VertexBuffer::updateGLBuffer() const {
  glBufferData(GL_ARRAY_BUFFER, _size*_length, _buffer, GL_STATIC_DRAW);
  _dirtyBuffer = false;
}

void VertexBuffer::genGLBuffer() { glGenBuffers(1, &_renderId); }

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