#include "Material.h"

unsigned int Material::s_idCounter = 0;
Material* Material::s_default = nullptr;

Material* Material::loadDefaultMaterial()
{ 
  return new Material(Shader::getDefault());
}
