#include "BasicMaterial.h"

#include "util/FileUtilities.h"

namespace px::rendering {
BasicMaterial::BasicMaterial(Shader* shader) {
  _shader = shader;
  _shader->grab();
}

BasicMaterial ::~BasicMaterial() { _shader->drop(); }

Shader* BasicMaterial::getShader() const { return _shader; }
}  // namespace px::rendering