#include <stdint.h>
#include <string>
#include <filesystem>

#include "PxeGLAsset.h"
#include "PxeBuffer.h"
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
#include "glm/gtc/type_ptr.hpp"

#ifndef PXENGINE_SHADER_H_
#define PXENGINE_SHADER_H_
namespace pxengine {
	// TODO Add PxeShader description
	class PxeShader : public PxeGLAsset
	{
	public:
		PxeShader() = default;
		virtual ~PxeShader() = default;

		virtual bool getValid() const = 0;
		virtual int32_t getUniformLocation(const std::string& name) = 0;
		virtual uint32_t getGlProgramId() const = 0;
		virtual const std::filesystem::path& getShaderPath() const = 0;

		// returns the number of times the PxeShader was initialized
		// useful to check if uniform locations need updating
		virtual uint32_t getAssetInitializationCount() const = 0;

		inline void setUniform1f(const std::string& name, const float value) { setUniform1fv(name, &value, 1); }
		inline void setUniform2f(const std::string& name, const glm::vec2& value) { setUniform2fv(name, glm::value_ptr(value), 1); }
		inline void setUniform3f(const std::string& name, const glm::vec3& value) { setUniform3fv(name, glm::value_ptr(value), 1); }
		inline void setUniform4f(const std::string& name, const glm::vec4& value) { setUniform4fv(name, glm::value_ptr(value), 1); }

		inline void setUniform1i(const std::string& name, const int32_t value) { setUniform1iv(name, &value, 1); }
		inline void setUniform2i(const std::string& name, const glm::i32vec2& value) { setUniform2iv(name, glm::value_ptr(value), 1); }
		inline void setUniform3i(const std::string& name, const glm::i32vec3& value) { setUniform3iv(name, glm::value_ptr(value), 1); }
		inline void setUniform4i(const std::string& name, const glm::i32vec4& value) { setUniform4iv(name, glm::value_ptr(value), 1); }

		inline void setUniform1ui(const std::string& name, const uint32_t value) { setUniform1uiv(name, &value, 1); }
		inline void setUniform2ui(const std::string& name, const glm::u32vec2& value) { setUniform2uiv(name, glm::value_ptr(value), 1); }
		inline void setUniform3ui(const std::string& name, const glm::u32vec3& value) { setUniform3uiv(name, glm::value_ptr(value), 1); }
		inline void setUniform4ui(const std::string& name, const glm::u32vec4& value) { setUniform4uiv(name, glm::value_ptr(value), 1); }

		inline void setUniformM2f(const std::string& name, const glm::mat2& value) { setUniformM2fv(name, glm::value_ptr(value), 1); }
		inline void setUniformM3f(const std::string& name, const glm::mat3& value) { setUniformM3fv(name, glm::value_ptr(value), 1); }
		inline void setUniformM4f(const std::string& name, const glm::mat4& value) { setUniformM4fv(name, glm::value_ptr(value), 1); }
		inline void setUniformM2x3f(const std::string& name, const glm::mat2x3& value) { setUniformM2x3fv(name, glm::value_ptr(value), 1); }
		inline void setUniformM3x2f(const std::string& name, const glm::mat3x2& value) { setUniformM3x2fv(name, glm::value_ptr(value), 1); }
		inline void setUniformM2x4f(const std::string& name, const glm::mat2x4& value) { setUniformM2x4fv(name, glm::value_ptr(value), 1); }
		inline void setUniformM4x2f(const std::string& name, const glm::mat4x2& value) { setUniformM4x2fv(name, glm::value_ptr(value), 1); }
		inline void setUniformM3x4f(const std::string& name, const glm::mat3x4& value) { setUniformM3x4fv(name, glm::value_ptr(value), 1); }
		inline void setUniformM4x3f(const std::string& name, const glm::mat4x3& value) { setUniformM4x3fv(name, glm::value_ptr(value), 1); }


		inline void setUniform1fv(const std::string& name, const float* values, uint32_t count) { setUniform1fv(getUniformLocation(name), values, count); }
		inline void setUniform2fv(const std::string& name, const float* values, uint32_t count) { setUniform2fv(getUniformLocation(name), values, count); }
		inline void setUniform3fv(const std::string& name, const float* values, uint32_t count) { setUniform3fv(getUniformLocation(name), values, count); }
		inline void setUniform4fv(const std::string& name, const float* values, uint32_t count) { setUniform4fv(getUniformLocation(name), values, count); }

		inline void setUniform1iv(const std::string& name, const int32_t* values, uint32_t count) { setUniform1iv(getUniformLocation(name), values, count); }
		inline void setUniform2iv(const std::string& name, const int32_t* values, uint32_t count) { setUniform2iv(getUniformLocation(name), values, count); }
		inline void setUniform3iv(const std::string& name, const int32_t* values, uint32_t count) { setUniform3iv(getUniformLocation(name), values, count); }
		inline void setUniform4iv(const std::string& name, const int32_t* values, uint32_t count) { setUniform4iv(getUniformLocation(name), values, count); }

		inline void setUniform1uiv(const std::string& name, const uint32_t* values, uint32_t count) { setUniform1uiv(getUniformLocation(name), values, count); }
		inline void setUniform2uiv(const std::string& name, const uint32_t* values, uint32_t count) { setUniform2uiv(getUniformLocation(name), values, count); }
		inline void setUniform3uiv(const std::string& name, const uint32_t* values, uint32_t count) { setUniform3uiv(getUniformLocation(name), values, count); }
		inline void setUniform4uiv(const std::string& name, const uint32_t* values, uint32_t count) { setUniform4uiv(getUniformLocation(name), values, count); }

		inline void setUniformM2fv(const std::string& name, const float* values, uint32_t count) { setUniformM2fv(getUniformLocation(name), values, count); }
		inline void setUniformM3fv(const std::string& name, const float* values, uint32_t count) { setUniformM3fv(getUniformLocation(name), values, count); }
		inline void setUniformM4fv(const std::string& name, const float* values, uint32_t count) { setUniformM4fv(getUniformLocation(name), values, count); }
		inline void setUniformM2x3fv(const std::string& name, const float* values, uint32_t count) { setUniformM2x3fv(getUniformLocation(name), values, count); }
		inline void setUniformM3x2fv(const std::string& name, const float* values, uint32_t count) { setUniformM3x2fv(getUniformLocation(name), values, count); }
		inline void setUniformM2x4fv(const std::string& name, const float* values, uint32_t count) { setUniformM2x4fv(getUniformLocation(name), values, count); }
		inline void setUniformM4x2fv(const std::string& name, const float* values, uint32_t count) { setUniformM4x2fv(getUniformLocation(name), values, count); }
		inline void setUniformM3x4fv(const std::string& name, const float* values, uint32_t count) { setUniformM3x4fv(getUniformLocation(name), values, count); }
		inline void setUniformM4x3fv(const std::string& name, const float* values, uint32_t count) { setUniformM4x3fv(getUniformLocation(name), values, count); }


		inline void setUniform1f(const int32_t location, const float value) { setUniform1fv(location, &value, 1); }
		inline void setUniform2f(const int32_t location, const glm::vec2& value) { setUniform2fv(location, glm::value_ptr(value), 1); }
		inline void setUniform3f(const int32_t location, const glm::vec3& value) { setUniform3fv(location, glm::value_ptr(value), 1); }
		inline void setUniform4f(const int32_t location, const glm::vec4& value) { setUniform4fv(location, glm::value_ptr(value), 1); }

		inline void setUniform1i(const int32_t location, const int32_t value) { setUniform1iv(location, &value, 1); }
		inline void setUniform2i(const int32_t location, const glm::i32vec2& value) { setUniform2iv(location, glm::value_ptr(value), 1); }
		inline void setUniform3i(const int32_t location, const glm::i32vec3& value) { setUniform3iv(location, glm::value_ptr(value), 1); }
		inline void setUniform4i(const int32_t location, const glm::i32vec4& value) { setUniform4iv(location, glm::value_ptr(value), 1); }

		inline void setUniform1ui(const int32_t location, const uint32_t value) { setUniform1uiv(location, &value, 1); }
		inline void setUniform2ui(const int32_t location, const glm::u32vec2& value) { setUniform2uiv(location, glm::value_ptr(value), 1); }
		inline void setUniform3ui(const int32_t location, const glm::u32vec3& value) { setUniform3uiv(location, glm::value_ptr(value), 1); }
		inline void setUniform4ui(const int32_t location, const glm::u32vec4& value) { setUniform4uiv(location, glm::value_ptr(value), 1); }

		inline void setUniformM2f(const int32_t location, const glm::mat2& value) { setUniformM2fv(location, glm::value_ptr(value), 1); }
		inline void setUniformM3f(const int32_t location, const glm::mat3& value) { setUniformM3fv(location, glm::value_ptr(value), 1); }
		inline void setUniformM4f(const int32_t location, const glm::mat4& value) { setUniformM4fv(location, glm::value_ptr(value), 1); }
		inline void setUniformM2x3f(const int32_t location, const glm::mat2x3& value) { setUniformM2x3fv(location, glm::value_ptr(value), 1); }
		inline void setUniformM3x2f(const int32_t location, const glm::mat3x2& value) { setUniformM3x2fv(location, glm::value_ptr(value), 1); }
		inline void setUniformM2x4f(const int32_t location, const glm::mat2x4& value) { setUniformM2x4fv(location, glm::value_ptr(value), 1); }
		inline void setUniformM4x2f(const int32_t location, const glm::mat4x2& value) { setUniformM4x2fv(location, glm::value_ptr(value), 1); }
		inline void setUniformM3x4f(const int32_t location, const glm::mat3x4& value) { setUniformM3x4fv(location, glm::value_ptr(value), 1); }
		inline void setUniformM4x3f(const int32_t location, const glm::mat4x3& value) { setUniformM4x3fv(location, glm::value_ptr(value), 1); }


		virtual void setUniform1fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniform2fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniform3fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniform4fv(const int32_t location, const float* values, uint32_t count) = 0;

		virtual void setUniform1iv(const int32_t location, const int32_t* values, uint32_t count) = 0;
		virtual void setUniform2iv(const int32_t location, const int32_t* values, uint32_t count) = 0;
		virtual void setUniform3iv(const int32_t location, const int32_t* values, uint32_t count) = 0;
		virtual void setUniform4iv(const int32_t location, const int32_t* values, uint32_t count) = 0;

		virtual void setUniform1uiv(const int32_t location, const uint32_t* values, uint32_t count) = 0;
		virtual void setUniform2uiv(const int32_t location, const uint32_t* values, uint32_t count) = 0;
		virtual void setUniform3uiv(const int32_t location, const uint32_t* values, uint32_t count) = 0;
		virtual void setUniform4uiv(const int32_t location, const uint32_t* values, uint32_t count) = 0;

		virtual void setUniformM2fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniformM3fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniformM4fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniformM2x3fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniformM3x2fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniformM2x4fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniformM4x2fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniformM3x4fv(const int32_t location, const float* values, uint32_t count) = 0;
		virtual void setUniformM4x3fv(const int32_t location, const float* values, uint32_t count) = 0;



		PxeShader(const PxeShader& other) = delete;
		PxeShader operator=(const PxeShader& other) = delete;
	private:

	};
}
#endif // !PXENGINE_SHADER_H_
