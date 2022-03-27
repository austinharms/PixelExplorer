#ifndef RENDER_MATERIAL_H
#define RENDER_MATERIAL_H
#include <stdint.h>

#include "RefCounted.h"
#include "Shader.h"
namespace px::rendering {
class Material : public RefCounted {
 public:
  inline Material() : _id(++s_idCounter) {}
  inline virtual ~Material() {}
  inline uint32_t getId() const { return _id; }

  virtual Shader* getShader() const = 0;
  virtual void bind() {}

 private:
  static uint32_t s_idCounter;
  uint32_t _id;
};
}  // namespace px::rendering
#endif  // !RENDER_MATERIAL_H
