#include "Material.h"

unsigned int Material::s_idCounter = 0;
Material* Material::s_default = nullptr;

Material* Material::loadDefaultMaterial()
{ 
  Shader* shader = Shader::getDefault();
  assert(shader->isValid());
  return new Material(shader);
}
