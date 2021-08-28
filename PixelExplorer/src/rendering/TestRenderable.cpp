#include "TestRenderable.h"
#include "glm/gtx/transform.hpp"

TestRenderable::TestRenderable() : Renderable(Material::getDefault()), _position(0.0f, 0.0f, -10.0f), _rotation(0.0f) {
  //_transform = glm::mat4(
  //  1.0f, 0.0f, 0.0f, 0.0f,
  //  0.0f, 1.0f, 0.0f, 0.0f,
  //  0.0f, 0.0f, 1.0f, 0.0f,
  //  0.0f, 0.0f,-10.0f, 1.0f
  //  );

  float vertices[24] = {
    -0.5f, -0.5f, -0.5f, //0
     0.5f, -0.5f, -0.5f, //1
     0.5f,  0.5f, -0.5f, //2
    -0.5f,  0.5f, -0.5f, //3
    -0.5f, -0.5f,  0.5f, //4
     0.5f, -0.5f,  0.5f, //5
     0.5f,  0.5f,  0.5f, //6
    -0.5f,  0.5f,  0.5f  //7
  };

  unsigned short indices[36] = {
    0, 2, 1, 0, 3, 2, //Front
    4, 5, 6, 4, 6, 7, //Back
    0, 4, 7, 0, 7, 3, //Left
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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 18 * sizeof(unsigned short), indices,
               GL_STATIC_DRAW);
}

TestRenderable::~TestRenderable() { 
  glDeleteVertexArrays(1, &_vertexArrayId);
  glDeleteBuffers(1, &_vertexBufferId);
  glDeleteBuffers(1, &_indexBufferId);
}

bool TestRenderable::onPreRender(float deltaTime, float* cameraPos,
                                 float* cameraRotation) {
  //_rotation.x += deltaTime;
  _rotation.y += deltaTime;
  return _visible;
}

void TestRenderable::onRender() {
  glBindVertexArray(_vertexArrayId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
