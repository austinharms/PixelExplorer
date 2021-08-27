#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>

#include "RefCounted.h"
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"

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

  bool isValid() const { return _renderId != 0; }

  static Shader* getDefault() {
    if (s_default == nullptr) s_default = loadDefaultShader();
    return s_default;
  }

 private:
  unsigned int _renderId;
  std::unordered_map<const char*, int> _uniforms;
  int getUniformLocation(const char* name);
  static unsigned int compileShader(unsigned int type,
                                    const std::string source);
  static unsigned int createProgram(const std::string path);
  static Shader* s_default;
  static Shader* loadDefaultShader();
};
#endif  // !SHADER_H