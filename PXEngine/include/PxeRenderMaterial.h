#include <stdint.h>
#include <string>
#include <unordered_map>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/mat2x3.hpp"
#include "glm/mat3x2.hpp"
#include "glm/mat2x4.hpp"
#include "glm/mat4x2.hpp"
#include "glm/mat3x4.hpp"
#include "glm/mat4x3.hpp"
#include "PxeRefCount.h"
#include "PxeBuffer.h"
#include "PxeShader.h"

#ifndef PXENGINE_RENDER_MATERIAL_H_
#define PXENGINE_RENDER_MATERIAL_H_
namespace pxengine {
	class PxeRenderMaterial : public PxeRefCount
	{
	public:
		PxeRenderMaterial(PxeShader& shader);
		virtual ~PxeRenderMaterial();

		void setProperty1f(const std::string& name, const float value);
		void setProperty2f(const std::string& name, const glm::vec2& value);
		void setProperty3f(const std::string& name, const glm::vec3& value);
		void setProperty4f(const std::string& name, const glm::vec4& value);

		void setProperty1i(const std::string& name, const int32_t value);
		void setProperty2i(const std::string& name, const glm::i32vec2& value);
		void setProperty3i(const std::string& name, const glm::i32vec3& value);
		void setProperty4i(const std::string& name, const glm::i32vec4& value);

		void setProperty1ui(const std::string& name, const uint32_t value) ;
		void setProperty2ui(const std::string& name, const glm::u32vec2& value);
		void setProperty3ui(const std::string& name, const glm::u32vec3& value);
		void setProperty4ui(const std::string& name, const glm::u32vec4& value);

		void setPropertyM2f(const std::string& name, const   glm::mat2& value);
		void setPropertyM3f(const std::string& name, const   glm::mat3& value);
		void setPropertyM4f(const std::string& name, const   glm::mat4& value);
		void setPropertyM2x3f(const std::string& name, const glm::mat2x3& value);
		void setPropertyM3x2f(const std::string& name, const glm::mat3x2& value);
		void setPropertyM2x4f(const std::string& name, const glm::mat2x4& value);
		void setPropertyM4x2f(const std::string& name, const glm::mat4x2& value);
		void setPropertyM3x4f(const std::string& name, const glm::mat3x4& value);
		void setPropertyM4x3f(const std::string& name, const glm::mat4x3& value);

		void setProperty1fv(const std::string& name, const float* values, uint32_t count);
		void setProperty2fv(const std::string& name, const float* values, uint32_t count);
		void setProperty3fv(const std::string& name, const float* values, uint32_t count);
		void setProperty4fv(const std::string& name, const float* values, uint32_t count);

		void setProperty1iv(const std::string& name, const int32_t* values, uint32_t count);
		void setProperty2iv(const std::string& name, const int32_t* values, uint32_t count);
		void setProperty3iv(const std::string& name, const int32_t* values, uint32_t count);
		void setProperty4iv(const std::string& name, const int32_t* values, uint32_t count);

		void setProperty1uiv(const std::string& name, const uint32_t* values, uint32_t count);
		void setProperty2uiv(const std::string& name, const uint32_t* values, uint32_t count);
		void setProperty3uiv(const std::string& name, const uint32_t* values, uint32_t count);
		void setProperty4uiv(const std::string& name, const uint32_t* values, uint32_t count);

		void setPropertyM2fv(const std::string& name, const float* values, uint32_t count);
		void setPropertyM3fv(const std::string& name, const float* values, uint32_t count);
		void setPropertyM4fv(const std::string& name, const float* values, uint32_t count);
		void setPropertyM2x3fv(const std::string& name, const float* values, uint32_t count);
		void setPropertyM3x2fv(const std::string& name, const float* values, uint32_t count);
		void setPropertyM2x4fv(const std::string& name, const float* values, uint32_t count);
		void setPropertyM4x2fv(const std::string& name, const float* values, uint32_t count);
		void setPropertyM3x4fv(const std::string& name, const float* values, uint32_t count);
		void setPropertyM4x3fv(const std::string& name, const float* values, uint32_t count);

		// note this method assumes the PxeShader (must be the same one used to construct the PxeRenderMaterial) is already bound and there is a valid gl context bound
		void applyMaterial();

		PxeShader& getShader() const;

		PxeRenderMaterial(const PxeRenderMaterial& other) = delete;
		PxeRenderMaterial operator=(const PxeRenderMaterial& other) = delete;

	protected:
		struct PxeRenderMaterialValue
		{
			enum class PxePropertyType : uint8_t
			{
				NONE,
				INT1,
				INT2,
				INT3,
				INT4,
				UINT1,
				UINT2,
				UINT3,
				UINT4,
				FLOAT1,
				FLOAT2,
				FLOAT3,
				FLOAT4,
				MAT2,
				MAT3,
				MAT4,
				MAT2X3,
				MAT3X2,
				MAT2X4,
				MAT4X2,
				MAT3X4,
				MAT4X3,
			};

			uint32_t ValueCount;
			int32_t UniformLocation;
			PxePropertyType PropertyType;

			union PxePropertyValue
			{
				int32_t i32;
				glm::i32vec2 i32vec2;
				glm::i32vec3 i32vec3;
				glm::i32vec4 i32vec4;
				uint32_t ui32;
				glm::u32vec2 ui32vec2;
				glm::u32vec3 ui32vec3;
				glm::u32vec4 ui32vec4;

				float f;
				glm::vec2 fvec2;
				glm::vec3 fvec3;
				glm::vec4 fvec4;

				glm::mat2 fmat2;
				glm::mat3 fmat3;
				glm::mat4 fmat4;
				glm::mat2x3 fmat2x3;
				glm::mat3x2 fmat3x2;
				glm::mat2x4 fmat2x4;
				glm::mat4x2 fmat4x2;
				glm::mat3x4 fmat3x4;
				glm::mat4x3 fmat4x3;

				PxeBuffer* buffer;
			} Value;

			PxeRenderMaterialValue() = default;

			PxeRenderMaterialValue(uint32_t count, PxePropertyType type, PxePropertyValue value) {
				ValueCount = count;
				PropertyType = type;
				Value = value;
				UniformLocation = -1;
			}

			PxeRenderMaterialValue(const PxeRenderMaterialValue& other) {
				Value = other.Value;
				ValueCount = other.ValueCount;
				PropertyType = other.PropertyType;
				UniformLocation = other.UniformLocation;
				if (ValueCount > 1) {
					Value.buffer->grab();
				}
			}

			PxeRenderMaterialValue& operator =(const PxeRenderMaterialValue& other) {
				Value = other.Value;
				ValueCount = other.ValueCount;
				PropertyType = other.PropertyType;
				UniformLocation = other.UniformLocation;
				if (ValueCount > 1) {
					Value.buffer->grab();
				}

				return *this;
			}

			~PxeRenderMaterialValue() {
				if (ValueCount > 1) {
					Value.buffer->drop();
					Value.i32 = 0;
				}
			}
		};

	private:
		void updatePropertyLocations();

		std::unordered_map<std::string, PxeRenderMaterialValue> _materialProperties;
		PxeShader& _shader;
		uint32_t _lastShaderCount;
	};
}
#endif // !PXENGINE_RENDER_MATERIAL_H_