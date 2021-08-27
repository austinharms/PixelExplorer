#include "TestRenderable.h"

TestRenderable::TestRenderable() : Renderable(Material::getDefault()), _transform(1.0f) {
  float vertices[24] = {
    0,0,0, //0
    1,0,0, //1
    1,1,0, //2
    0,1,0, //3
    0,0,1, //4
    1,0,1, //5
    1,1,1, //6
    0,1,1  //7
  };

  unsigned short indices[36] = {
    0,2,1, 0,3,2
  };

  _visible = true;
  glGenVertexArrays(1, &_vertexArrayId);
  glBindVertexArray(_vertexArrayId);
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), indices,
               GL_STATIC_DRAW);
}

TestRenderable::~TestRenderable() { 
  glDeleteVertexArrays(1, &_vertexArrayId);
  glDeleteBuffers(1, &_vertexBufferId);
  glDeleteBuffers(1, &_indexBufferId);
}

bool TestRenderable::onPreRender(float deltaTime, float* cameraPos,
                                 float* cameraRotation) {
  return _visible;
}

void TestRenderable::onRender() {
  glBindVertexArray(_vertexArrayId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
