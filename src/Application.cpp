#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include<fstream>
#include<iostream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "stb_image/stb_image.h"

static unsigned int CompileShader(unsigned int type, const std::string source) {
  unsigned int id = glCreateShader(type);
  const char* src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int res;
  glGetShaderiv(id, GL_COMPILE_STATUS, &res);
  if (res == GL_FALSE) {
    std::cout << "Failed to compile shader" << std::endl;
    glDeleteShader(id);
    return 0;
  }

  return id;
}

static unsigned int CreateProgram(const std::string path) { 
  std::ifstream stream(path);

  enum class ShaderType {
    NONE = -1, VERTEX = 0, FRAGMENT = 1
  };

  std::string line;
  std::stringstream ss[2];
  ShaderType type = ShaderType::NONE;
  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        type = ShaderType::VERTEX;
      } else if (line.find("fragment") != std::string::npos) {
        type = ShaderType::FRAGMENT;
      } else {
        type = ShaderType::NONE;
      }
    } else {
      ss[(int)type] << line << "\n";
    }
  }

  unsigned int program = glCreateProgram();
  unsigned int vs =
      CompileShader(GL_VERTEX_SHADER, ss[(int)ShaderType::VERTEX].str());
  unsigned int fs =
      CompileShader(GL_FRAGMENT_SHADER, ss[(int)ShaderType::FRAGMENT].str());
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);
  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

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

  //load texture
  int tWidth = 0;
  int tHeight = 0;
  int BPP = 0;
  stbi_set_flip_vertically_on_load(1);
  void* imgBuffer = stbi_load("./res/textures/textures.png", &tWidth, &tHeight, &BPP, 4);
  unsigned int texId;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tWidth, tHeight, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, imgBuffer);
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(imgBuffer);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texId);

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

  unsigned int shader = CreateProgram("./res/shaders/Basic.shader");
  glUseProgram(shader);
  int loc = glGetUniformLocation(shader, "u_Texture");
  glUniform1i(loc, 0);

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

  unsigned int indexBuff;
  glGenBuffers(1, &indexBuff);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), index,
               GL_STATIC_DRAW);

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

  glfwTerminate();
  return 0;
}