#include "BasicMaterial.h"
#include "util/FileUtilities.h"

namespace px::rendering {
BasicMaterial::BasicMaterial(Shader* shader) {
  if (shader != nullptr) {
    _shader = shader;
    _shader->grab();
  } else {
    _shader = new Shader(util::FileUtilities::getAssetDirectory() +
                         "shaders/basic.shader");
  }
}

BasicMaterial ::~BasicMaterial() {
  if (_shader != nullptr) _shader->drop();
}

Shader* BasicMaterial::getShader() const { return _shader; }
}  // namespace px::rendering