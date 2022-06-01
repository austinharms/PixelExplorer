#include <string>
#include <stdint.h>

#include "Shader.h"

#ifndef PIXELEXPLORE_RENDERING_MATERIALPROPERTIES_H_
#define PIXELEXPLORE_RENDERING_MATERIALPROPERTIES_H_
namespace pixelexplore::rendering {
	class MaterialProperty
	{
	public:
		virtual void applyProperty(Shader* shader, const std::string& name) = 0;
	};

	class floatProperty : public MaterialProperty
	{
	public:
		float value;

		inline floatProperty(float value) { this->value = value; }
		void applyProperty(Shader* shader, const std::string& name) {
			shader->setUniform1f(name, value);
		}
	};

	class int32Property : public MaterialProperty
	{
	public:
		int32_t value;

		inline int32Property(int32_t value) { this->value = value; }
		void applyProperty(Shader* shader, const std::string& name) {
			shader->setUniform1i(name, value);
		}
	};
}
#endif // !PIXELEXPLORE_RENDERING_MATERIALPROPERTIES_H_