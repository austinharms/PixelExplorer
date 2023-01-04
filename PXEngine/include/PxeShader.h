#include <stdint.h>
#include <string>
#include <filesystem>

#include "PxeGLAsset.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

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
		virtual void setUniform1i(const std::string& name, const int32_t value) = 0;
		virtual void setUniform1f(const std::string& name, const float value) = 0;
		virtual void setUniform2fv(const std::string& name, const glm::vec2& value) = 0;
		virtual void setUniform3fv(const std::string& name, const glm::vec3& value) = 0;
		virtual void setUniform4fv(const std::string& name, const glm::vec4& value) = 0;
		virtual void setUniformm4fv(const std::string& name, const glm::mat4& value) = 0;
		virtual int32_t getUniformLocation(const std::string& name) = 0;
		virtual uint32_t getGlProgramId() const = 0;
		virtual const std::filesystem::path& getShaderPath() const = 0;

		PxeShader(const PxeShader& other) = delete;
		PxeShader operator=(const PxeShader& other) = delete;
	private:

	};
}
#endif // !PXENGINE_SHADER_H_
