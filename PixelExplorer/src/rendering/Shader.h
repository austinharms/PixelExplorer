#ifndef SHADER_H
#define SHADER_H
#include <stdint.h>
#include <string>
#include <unordered_map>

#include "RefCounted.h"
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"
namespace px::rendering {
class Shader : public RefCounted {
 public:
  Shader(const std::string shaderFilepath);
  virtual ~Shader();
  void bind() const;
  void unbind() const;
  void setUniform1i(const char* name, int32_t value);
  void setUniformm4(const char* name, glm::mat4 value);
  void setUniformv4(const char* name, glm::vec4 value);

  inline uint32_t getGLID() const { return _renderId; }
  inline bool isValid() const { return _renderId != 0; }

 private:
  uint32_t _renderId;
  std::unordered_map<const char*, int32_t> _uniforms;
  int getUniformLocation(const char* name);
  static uint32_t compileShader(uint32_t type,
                                    const std::string source);
  static uint32_t createProgram(const std::string path);
};
}  // namespace px::rendering
#endif  // !SHADER_H