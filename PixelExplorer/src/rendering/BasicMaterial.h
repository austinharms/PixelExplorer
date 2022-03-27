#ifndef BASICMATERIAL_H
#define BASICMATERIAL_H
#include <stdint.h>

#include "Material.h"
#include "Shader.h"
namespace px::rendering {
class BasicMaterial : public Material {
 public:
  BasicMaterial(Shader* shader = nullptr);
  virtual ~BasicMaterial();
  Shader* getShader() const;

 private:
  Shader* _shader;
};
}  // namespace px::rendering
#endif