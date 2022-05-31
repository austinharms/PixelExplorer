#include <stdint.h>
#include <unordered_map>
#include <string>

#include "RefCount.h"
#include "Shader.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#ifndef PIXELEXPLORE_RENDERING_MATERIAL_H_
#define PIXELEXPLORE_RENDERING_MATERIAL_H_
namespace pixelexplore::rendering {
	class Material : public RefCount
	{
	public:
		enum class PropertyType
		{
			FLOAT = 0,
			INT32,
			GLM_VEC2,
			GLM_VEC3,
			GLM_VEC4,
			GLM_MAT4,
		};

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

	protected:
		struct MaterialProperty
		{
			uint32_t typeId;
			void* valuePtr;
			~MaterialProperty() {
				free(valuePtr);
			}
		};

		virtual void applyProperty(Shader* shader, MaterialProperty p);
	private:

	};
}
#endif // !PIXELEXPLORE_RENDERING_MATERIAL_H_
