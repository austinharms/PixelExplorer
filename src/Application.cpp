#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Material.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"

int main(void) {
  Renderer* renderer = new Renderer(800, 600, "Test");
  if (!renderer->renderInit()) return -1;
  renderer->setCameraTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -20.0f)));
  Shader* shader = new Shader("./res/shaders/Basic.shader");
  Texture* text = new Texture("./res/textures/textures.png");
  renderer->useShader(shader); 
  shader->drop();
  Material* mat = new Material();
  mat->setTexture(text);
  mat->setColor(1.0f, 1.0f, 1.0f, 1.0f);
  text->drop();
  Mesh* mesh = new Mesh();
  mesh->setMaterial(mat);
  mat->drop();
  mesh->setIndexCount(36);
  mesh->setVertexCount(24);

  float tw = (1.0f / 96.0f)*16.0f;
  float th = (1.0f / 64.0f)*16.0f;

  float block[] = {
    //Front Face
    -0.5f, -0.5f,  0.5f, tw * 2, th * 1,
    -0.5f,  0.5f,  0.5f, tw * 2, th * 0,
     0.5f,  0.5f,  0.5f, tw * 3, th * 0,
     0.5f, -0.5f,  0.5f, tw * 3, th * 1,

    //Back Face
    -0.5f, -0.5f, -0.5f, tw * 4, th * 1,
    -0.5f,  0.5f, -0.5f, tw * 4, th * 0,
     0.5f,  0.5f, -0.5f, tw * 3, th * 0,
     0.5f, -0.5f, -0.5f, tw * 3, th * 1,

    //Left Face
    -0.5f, -0.5f, -0.5f, tw * 4, th * 1,
    -0.5f,  0.5f, -0.5f, tw * 4, th * 0,
    -0.5f,  0.5f,  0.5f, tw * 5, th * 0,
    -0.5f, -0.5f,  0.5f, tw * 5, th * 1,

    //Right Face
     0.5f, -0.5f, -0.5f, tw * 6, th * 1,
     0.5f,  0.5f, -0.5f, tw * 6, th * 0,
     0.5f,  0.5f,  0.5f, tw * 5, th * 0,
     0.5f, -0.5f,  0.5f, tw * 5, th * 1,

    //Top Face
    -0.5f,  0.5f, -0.5f, tw * 0, th * 0,
    -0.5f,  0.5f,  0.5f, tw * 0, th * 1,
     0.5f,  0.5f,  0.5f, tw * 1, th * 1,
     0.5f,  0.5f, -0.5f, tw * 1, th * 0,

    //Bottom Face
    -0.5f, -0.5f, -0.5f, tw * 1, th * 1,
    -0.5f, -0.5f,  0.5f, tw * 1, th * 0,
     0.5f, -0.5f,  0.5f, tw * 2, th * 0,
     0.5f, -0.5f, -0.5f, tw * 2, th * 1,
  };

  for (short i = 0; i < 120; i += 5) {
    mesh->setVertexPosition(i / 5, block[i], block[i + 1], block[i + 2]);
    mesh->setVertexUV(i / 5, block[i + 3], block[i + 4]);
  }

  unsigned short index[36] = {
       0,  3,  2,  2,  1,  0,  // Front Face
       6,  7,  4,  4,  5,  6,  // Back Face
       8, 11, 10, 10,  9,  8,  // Left Face
      14, 15, 12, 12, 13, 14,  // Right Face
      18, 19, 16, 16, 17, 18,  // Top Face
      20, 23, 22, 22, 21, 20,  // Bottom Face
  };

  for (short i = 0; i < 36; ++i) {
    mesh->setIndex(i, index[i]);
  }

  mesh->updateBuffers();
  renderer->addMesh(mesh);

  while (renderer->windowOpen()) {
    renderer->render();
  }

  renderer->drop();
  mesh->drop();
  return 0;
}