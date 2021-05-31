#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"


int main(void) {

  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) return -1;
  Renderer* renderer = new Renderer();
  Shader* shader = new Shader("./res/shaders/Basic.shader");
  Texture* text = new Texture("./res/textures/textures.png");
  text->bind(0);
  shader->setUniform1i("u_Texture", 0);
  renderer->useShader(shader); 

  Mesh* mesh = new Mesh();
  mesh->setIndexCount(6);
  mesh->setVertexCount(4);

  float block[20] = {
  //Front Face
    0, 0, 0, 0, 0,
    0, 1, 0, 0, 0.25f,
    1, 1, 0, 0.16666666f, 0.25f,
    1, 0, 0, 0.16666666f, 0
  };

  for (short i = 0; i < 20; i += 5) {
    mesh->setVertexPosition(i / 5, block[i], block[i + 1], block[i + 2]);
    mesh->setVertexUV(i / 5, block[i + 3], block[i + 4]);
  }

  unsigned short index[6] = {0, 3, 2, 2, 1, 0};

  for (short i = 0; i < 6; ++i) {
    mesh->setIndex(i, index[i]);
  }

  mesh->updateBuffers();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    renderer->drawMesh(mesh);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  renderer->drop();
  shader->drop();
  text->drop();
  mesh->drop();
  glfwTerminate();
  return 0;
}