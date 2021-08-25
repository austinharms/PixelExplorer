#include "Material.h"

unsigned int Material::s_idCounter = 0;
const Material* Material::DEFAULT = Material::loadDefaultMaterial();

Material* Material::loadDefaultMaterial()
{ 
  assert(Shader::DEFAULT->isValid());
  return new Material(Shader::DEFAULT);
}
