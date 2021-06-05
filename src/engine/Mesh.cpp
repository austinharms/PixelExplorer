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
      _material(nullptr) {
  _vertexBuffer = new VertexBuffer();
  _indexBuffer = new IndexBuffer();
  _vertexArray = new VertexArray();
  VertexBufferAttrib* attribs[2] = {new VertexBufferAttrib(3, GL_FLOAT),
                                    new VertexBufferAttrib(2, GL_FLOAT)};
  _vertexArray->addVertexBuffer(_vertexBuffer, attribs, 2);
  attribs[0]->drop();
  attribs[1]->drop();

  //_transform = glm::scale(_transform, glm::vec3(5.0f));
  _transform = glm::translate(_transform, glm::vec3(0.0f));
  //_transform = glm::rotate(_transform, glm::radians(-90.0f), glm::vec3(1.0f,
  // 0.0f, 0.0f));
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
  _indices = new unsigned short[count];
  _indexCount = count;
}

void Mesh::setVertexCount(unsigned int count) {
  delete[] _vertices;
  _vertices = new float[count * 5];
  _vertexCount = count;
}

void Mesh::setIndex(unsigned int index, unsigned short value) {
  _indices[index] = value;
}

void Mesh::setVertexPosition(unsigned int index, float x, float y, float z) {
  _vertices[(index * 5)] = x;
  _vertices[(index * 5) + 1] = y;
  _vertices[(index * 5) + 2] = z;
}

void Mesh::setVertexUV(unsigned int index, float u, float v) {
  _vertices[(index * 5) + 3] = u;
  _vertices[(index * 5) + 4] = v;
}

void Mesh::updateBuffers() {
  _indexBuffer->updateIndices(sizeof(unsigned short), _indexCount, _indices);
  _vertexBuffer->updateVertices(sizeof(float), _vertexCount * 5, _vertices);
}

void Mesh::updateTransfrom(float dt) {
  //_transform = glm::rotate(_transform, 1.0f * dt, glm::vec3(0.0f, 1.0f,
  // 0.0f));
  _transform = glm::rotate(_transform, 1.0f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
}
