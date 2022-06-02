#include <string>
#include <stdint.h>

#include "Shader.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#ifndef PIXELEXPLORE_RENDERING_MATERIALPROPERTIES_H_
#define PIXELEXPLORE_RENDERING_MATERIALPROPERTIES_H_
namespace pixelexplorer::rendering {
	class MaterialProperty
	{
	public:
		virtual inline ~MaterialProperty() {}
		virtual void applyProperty(Shader* shader, const std::string& name) = 0;
	};

	class floatProperty : public MaterialProperty
	{
	public:
		float value;

		inline floatProperty(float value) { this->value = value; }
		virtual inline ~floatProperty() {}
		inline void applyProperty(Shader* shader, const std::string& name) {
			shader->setUniform1f(name, value);
		}
	};

	class int32Property : public MaterialProperty
	{
	public:
		int32_t value;

		inline int32Property(int32_t value) { this->value = value; }
		virtual inline ~int32Property() {}
		inline void applyProperty(Shader* shader, const std::string& name) {
			shader->setUniform1i(name, value);
		}
	};

	class vec2Property : public MaterialProperty
	{
	public:
		glm::vec2 value;

		inline vec2Property(glm::vec2 value) { this->value = value; }
		virtual inline ~vec2Property() {}
		inline void applyProperty(Shader* shader, const std::string& name) {
			shader->setUniform2fv(name, value);
		}
	};

	class vec3Property : public MaterialProperty
	{
	public:
		glm::vec3 value;

		inline vec3Property(glm::vec3 value) { this->value = value; }
		virtual inline ~vec3Property() {}
		inline void applyProperty(Shader* shader, const std::string& name) {
			shader->setUniform3fv(name, value);
		}
	};

	class vec4Property : public MaterialProperty
	{
	public:
		glm::vec4 value;

		inline vec4Property(glm::vec4 value) { this->value = value; }
		virtual inline ~vec4Property() {}
		inline void applyProperty(Shader* shader, const std::string& name) {
			shader->setUniform4fv(name, value);
		}
	};

	class mat4Property : public MaterialProperty
	{
	public:
		glm::mat4 value;

		inline mat4Property(glm::mat4 value) { this->value = value; }
		virtual inline ~mat4Property() {}
		inline void applyProperty(Shader* shader, const std::string& name) {
			shader->setUniformm4fv(name, value);
		}
	};
}
#endif // !PIXELEXPLORE_RENDERING_MATERIALPROPERTIES_H_