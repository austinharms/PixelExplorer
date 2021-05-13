#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) return -1;

  float block[12] = {
  //Front Face
    0, 0, 0,
    0, 1, 0,
    1, 1, 0,
    1, 0, 0
  };


  unsigned int vertBuff;
  glGenBuffers(1, &vertBuff);
  glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), block, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}