#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "Renderer.h"
#include "Texture.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferAttrib.h"
#include "Shader.h"


int main(void) {

  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(500, 500, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) return -1;
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(GLErrorCallback, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Texture* text = new Texture("./res/textures/textures.png");
  text->bind();

  float block[20] = {
  //Front Face
    0, 0, 0, 0, 0,
    0, 1, 0, 0, 0.25f,
    1, 1, 0, 0.16666666f, 0.25f,
    1, 0, 0, 0.16666666f, 0
  };

  unsigned short index[] = {
     0,3,2,
     2,1,0
  };

  IndexBuffer* iBuffer = new IndexBuffer(sizeof(unsigned short), 6, index);

  Shader* shader = new Shader("./res/shaders/Basic.shader");
  glUseProgram(shader->getGLID());
  int loc = glGetUniformLocation(shader->getGLID(), "u_Texture");
  glUniform1i(loc, 0);

  VertexArray* vArray = new VertexArray();
  vArray->bind();

  VertexBuffer* vBuffer = new VertexBuffer();
  vBuffer->updateVertices(20 * sizeof(float), block);
  //vBuffer->bind();
  VertexBufferAttrib* attribs[2] = {
      new VertexBufferAttrib(3, GL_FLOAT),
      new VertexBufferAttrib(2, GL_FLOAT)
  };
  vArray->addVertexBuffer(vBuffer, attribs, 2);
  vBuffer->drop();
  attribs[0]->drop();
  attribs[1]->drop();

  iBuffer->bind();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  text->drop();
  iBuffer->drop();
  vArray->drop();
  shader->drop();
  glfwTerminate();
  return 0;
}