#include "Mesh.h"

#include <glm/gtc/matrix_transform.hpp>

#include "VertexBufferAttrib.h"

unsigned int Mesh::s_nextId = 1;

Mesh::Mesh()
    : _transform(glm::mat4(1.0f)),
      _vertexBuffer(nullptr),
      _indexBuffer(nullptr),
      _vertexArray(nullptr),
      _indexCount(0),
      _vertexCount(0),
      _indices(nullptr),
      _vertices(nullptr),
      _id(s_nextId++),
      _material(nullptr),
      _attribStride(0) {
  _vertexBuffer = new VertexBuffer();
  _indexBuffer = new IndexBuffer();
  _vertexArray = new VertexArray();
  VertexBufferAttrib* attribs[2] = {new VertexBufferAttrib(3, GL_FLOAT),
                                    new VertexBufferAttrib(2, GL_FLOAT)};
  _vertexArray->addVertexBuffer(_vertexBuffer, attribs, 2);
  attribs[0]->drop();
  attribs[1]->drop();
  _attribStride = _vertexArray->getBufferComponentStride(0);
  _transform = glm::translate(_transform, glm::vec3(0.0f));
}

Mesh::Mesh(VertexBufferAttrib* customAttribs[], unsigned short attribCount)
    : _transform(glm::mat4(1.0f)),
      _vertexBuffer(nullptr),
      _indexBuffer(nullptr),
      _vertexArray(nullptr),
      _indexCount(0),
      _vertexCount(0),
      _indices(nullptr),
      _vertices(nullptr),
      _id(s_nextId++),
      _material(nullptr),
      _attribStride(0) {
  _vertexBuffer = new VertexBuffer();
  _indexBuffer = new IndexBuffer();
  _vertexArray = new VertexArray();
  VertexBufferAttrib** attribs = new VertexBufferAttrib*[attribCount + 2];
  attribs[0] = new VertexBufferAttrib(3, GL_FLOAT);
  attribs[1] = new VertexBufferAttrib(2, GL_FLOAT);
  for (unsigned short i = 0; i < attribCount; ++i)
    attribs[i + 2] = customAttribs[i];
  _vertexArray->addVertexBuffer(_vertexBuffer, attribs, attribCount + 2);
  attribs[0]->drop();
  attribs[1]->drop();
  delete[] attribs;
  _attribStride = _vertexArray->getBufferComponentStride(0);
  _transform = glm::translate(_transform, glm::vec3(0.0f));
}

Mesh::~Mesh() {
  _vertexBuffer->drop();
  _indexBuffer->drop();
  _vertexArray->drop();
  if (_material != nullptr) _material->drop();
  delete[] _vertices;
  delete[] _indices;
}

void Mesh::bind() const {
  _vertexArray->bind();
  _indexBuffer->bind();
}

void Mesh::unbind() const {
  _vertexArray->unbind();
  _indexBuffer->unbind();
}

void Mesh::setIndexCount(unsigned int count) {
  delete[] _indices;
  _indices = new unsigned int[count];
  _indexCount = count;
}

void Mesh::setVertexCount(unsigned int count) {
  delete[] _vertices;
  _vertices = new float[count * _attribStride];
  _vertexCount = count;
}

void Mesh::setIndex(unsigned int index, unsigned int value) {
  _indices[index] = value;
}

void Mesh::setVertexPosition(unsigned int index, float x, float y, float z) {
  setAttribVec3(0, index, x, y, z);
}

void Mesh::setVertexUV(unsigned int index, float u, float v) {
  setAttribVec2(1, index, u, v);
}

void Mesh::setAttribVec2(unsigned short attribIndex, unsigned int index,
                         float x, float y) {
  unsigned short offset =
      _vertexArray->getBufferAttribComponentOffset(0, attribIndex);
  _vertices[(index * _attribStride) + offset] = x;
  _vertices[(index * _attribStride) + offset + 1] = y;
}

void Mesh::setAttribVec3(unsigned short attribIndex, unsigned int index,
                         float x, float y, float z) {
  unsigned short offset =
      _vertexArray->getBufferAttribComponentOffset(0, attribIndex);
  _vertices[(index * _attribStride) + offset] = x;
  _vertices[(index * _attribStride) + offset + 1] = y;
  _vertices[(index * _attribStride) + offset + 2] = z;
}

void Mesh::updateBuffers() {
  _indexBuffer->updateIndices(sizeof(unsigned int), _indexCount, _indices);
  _vertexBuffer->updateVertices(sizeof(float), _vertexCount * _attribStride, _vertices);
}

void Mesh::updateTransfrom(float dt) {
  //_transform = glm::rotate(_transform, 1.0f * dt, glm::vec3(0.0f, 1.0f,
  // 0.0f));
  _transform = glm::rotate(_transform, 1.0f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
}
