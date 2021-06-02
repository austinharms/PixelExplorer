#pragma once
#include <string>
#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "RefCounted.h"

class Shader : public virtual RefCounted {
 public:
  Shader(const std::string shaderFilepath);
  virtual ~Shader();
  void bind() const;
  void unbind() const;
  unsigned int getGLID() const { return _renderId; }
  void setUniform1i(const char* name, int value);
  void setUniformm4(const char* name, glm::mat4 value);
  void setUniformv4(const char* name, glm::vec4 value);
 private:
  unsigned int _renderId;
  std::unordered_map<const char*, int> _uniforms;
  int getUniformLocation(const char* name);
  static unsigned int compileShader(unsigned int type,
                                    const std::string source);
  static unsigned int createProgram(const std::string path);
};

/*
#include<fstream>
#include<iostream>
#include <sstream>
#include <string>


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
*/