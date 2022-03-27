#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"
#include "Logger.h"
namespace px::rendering {
Shader::Shader(const std::string shaderFilepath) : _renderId(0) {
  _renderId = createProgram(shaderFilepath);
}

Shader::~Shader() { glDeleteProgram(_renderId); }

void Shader::bind() const { glUseProgram(_renderId); }

void Shader::unbind() const { glUseProgram(0); }

unsigned int Shader::compileShader(uint32_t type,
                                   const std::string source) {
  uint32_t id = glCreateShader(type);
  const char* src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int32_t res;
  glGetShaderiv(id, GL_COMPILE_STATUS, &res);
  if (res == GL_FALSE) {
    int32_t maxLength = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);
    std::string err =  "Failed to compile shader: ";
    for (std::vector<char>::const_iterator i = errorLog.begin();
         i != errorLog.end(); ++i)
      err += *i;
    Logger::error(err);
    glDeleteShader(id);
    return 0;
  }

  return id;
}

int Shader::getUniformLocation(const char* name) {
  auto uniform = _uniforms.find(name);
  if (uniform != _uniforms.end()) {
    return uniform->second;
  } else {
    int32_t u = glGetUniformLocation(_renderId, name);
    _uniforms.insert({name, u});
    return u;
  }
}

void Shader::setUniform1i(const char* name, int32_t value) {
  int32_t loc = getUniformLocation(name);
  if (loc == -1) return;
  glUniform1i(loc, value);
}

void Shader::setUniformm4(const char* name, glm::mat4 value) {
  int32_t loc = getUniformLocation(name);
  if (loc == -1) return;
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniformv4(const char* name, glm::vec4 value) {
  int32_t loc = getUniformLocation(name);
  if (loc == -1) return;
  glUniform4fv(loc, 1, glm::value_ptr(value));
}

uint32_t Shader::createProgram(const std::string path) {
  std::ifstream stream(path);

  enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

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
      ss[(int32_t)type] << line << "\n";
    }
  }

  uint32_t program = glCreateProgram();
  uint32_t vs =
      compileShader(GL_VERTEX_SHADER, ss[(int)ShaderType::VERTEX].str());
  uint32_t fs =
      compileShader(GL_FRAGMENT_SHADER, ss[(int)ShaderType::FRAGMENT].str());
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);
  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}
}  // namespace px::rendering