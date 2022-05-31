#include <stdint.h>
#include <unordered_map>
#include <string>

#include "RefCount.h"
#include "Shader.h"

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
		virtual void addProperty(const std::string& propertyName);
	private:

	};
}
#endif // !PIXELEXPLORE_RENDERING_MATERIAL_H_
