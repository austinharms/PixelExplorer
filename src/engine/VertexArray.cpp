#include "VertexArray.h"

VertexArray::VertexArray()
    : _renderId(0), _nextBufferId(0), _nextAttribIndex(0), _buffers() {
  genGLArray();
}

VertexArray::~VertexArray() {
  for (VertexBufferLayout* buf : _buffers) buf->drop();
  glDeleteVertexArrays(1, &_renderId);
}

void VertexArray::bind() const {
  for (VertexBufferLayout* buf : _buffers) buf->buffer->updateDirtyBuffer();
  glBindVertexArray(_renderId);
}

void VertexArray::unbind() const { glBindVertexArray(0); }

unsigned int VertexArray::addVertexBuffer(VertexBuffer* buffer,
                                          VertexBufferAttrib* layout[],
                                          short layoutAttribCount) {
  VertexBufferLayout* buffferLayout = new VertexBufferLayout(
      _nextBufferId++, _nextAttribIndex, buffer, layout, layoutAttribCount);
  _buffers.emplace_back(buffferLayout);
  bind();
  buffer->bind();
  for (VertexBufferAttrib* attrib : buffferLayout->layout) {
    glVertexAttribPointer(attrib->index, attrib->componentCount,
                          attrib->datatype, attrib->normalized,
                          buffferLayout->stride, (const void*)attrib->offset);
    glEnableVertexAttribArray(attrib->index);
  }

  buffer->unbind();
  return buffferLayout->id;
}

int VertexArray::getBufferStride(unsigned short bufferIndex) const {
  auto buffersI = _buffers.begin();
  if (bufferIndex > 0) buffersI += bufferIndex;
  return (*buffersI)->getStride();
}

int VertexArray::getBufferComponentStride(unsigned short bufferIndex) const {
  auto buffersI = _buffers.begin();
  if (bufferIndex > 0) buffersI += bufferIndex;
  return (*buffersI)->getComponentStride();
}

unsigned short VertexArray::getBufferAttribComponentOffset(
    unsigned short bufferIndex, unsigned short attribIndex) const {
  auto buffersI = _buffers.begin();
  if (bufferIndex > 0) buffersI += bufferIndex;
  return (*buffersI)->getAttribComponentOffset(attribIndex);
}

void VertexArray::genGLArray() { glGenVertexArrays(1, &_renderId); }

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