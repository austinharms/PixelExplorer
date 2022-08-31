#include <stdint.h>
#include <string>
#include <unordered_map>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "GLObject.h"

#ifndef PIXELEXPLORE_RENDERING_SHADER_H_
#define PIXELEXPLORE_RENDERING_SHADER_H_
namespace pixelexplorer::rendering {
	class Shader : public GLObject
	{
		friend class RenderWindow;
	public:
		virtual ~Shader();
		inline bool isValid() const { return _glId != 0; }
		inline const std::string getPath() const { return _path; }
		inline uint32_t getGLId() const { return _glId; }
		void setUniform1i(const std::string& name, const int32_t value);
		void setUniform1f(const std::string& name, const float value);
		void setUniform2fv(const std::string& name, const glm::vec2& value);
		void setUniform3fv(const std::string& name, const glm::vec3& value);
		void setUniform4fv(const std::string& name, const glm::vec4& value);
		void setUniformm4fv(const std::string& name, const glm::mat4& value);

	protected:
		void onInitialize() override;
		void onUpdate() override;
		void onTerminate() override;

	private:
		Shader(const std::string& path);
		uint32_t compileShader(uint32_t shaderType, const std::string& source) const;
		uint32_t loadShader(const std::string& path) const;
		int32_t getUniformLocation(const std::string& name);
		void bind() const;
		void unbind() const;

		uint32_t _glId;
		const std::string _path;
		std::unordered_map<std::string, int32_t> _uniforms;
	};
}
#endif // !PIXELEXPLORE_RENDERING_SHADER_H_
