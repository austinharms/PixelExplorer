#include <stdint.h>
#include <unordered_map>
#include <string>

#include "RefCount.h"
#include "Shader.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "MaterialProperties.h"

#ifndef PIXELEXPLORE_RENDERING_MATERIAL_H_
#define PIXELEXPLORE_RENDERING_MATERIAL_H_
namespace pixelexplore::rendering {
	class Material : public RefCount
	{
	public:
		Material();
		virtual ~Material();
		virtual void applyMaterial(Shader* shader);
		virtual void removeProperty(const std::string& propertyName);
		void addProperty(const std::string& propertyName, float value);
		void addProperty(const std::string& propertyName, int32_t value);
		void addProperty(const std::string& propertyName, glm::vec2 value);
		void addProperty(const std::string& propertyName, glm::vec3 value);
		void addProperty(const std::string& propertyName, glm::vec4 value);
		void addProperty(const std::string& propertyName, glm::mat4 value);
		void addProperty(const std::string& propertyName, MaterialProperty* prop);
	private:

	};
}
#endif // !PIXELEXPLORE_RENDERING_MATERIAL_H_
