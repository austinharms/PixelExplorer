#include <stdint.h>
#include <string>

#include "PxeShader.h"
#include "GL/glew.h"

#ifndef PXENGINE_NONPUBLIC_SHADER_H_
#define PXENGINE_NONPUBLIC_SHADER_H_
namespace pxengine {
	namespace nonpublic {
		// TODO Add all setUniform types
		class NpShader : public PxeShader
		{
		public:
			//############# PUBLIC API ##################

			// note this only checks for initial validation not for current validation
			bool getValid() const override;

			void setUniform1i(const std::string& name, const int32_t value) override;

			void setUniform1f(const std::string& name, const float value) override;

			void setUniform2fv(const std::string& name, const glm::vec2& value) override;

			void setUniform3fv(const std::string& name, const glm::vec3& value) override;

			void setUniform4fv(const std::string& name, const glm::vec4& value) override;

			void setUniformm4fv(const std::string& name, const glm::mat4& value) override;			

			int32_t getUniformLocation(const std::string& name) override;

			uint32_t getGlProgramId() const override;

			const std::string& getShaderPath() const override;


			//############# PRIVATE API ##################

			NpShader(const std::string& path);

			virtual ~NpShader();

			// checks the current validation status of the program
			// note only logs on invalid programs and requires a gl context
			bool validateProgram();

		protected:
			enum class PxeShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2, SHADER_COUNT = 3 };
			static uint32_t compileShader(PxeShaderType shaderType, const std::string& source);
			static uint32_t loadShaderFile(const std::string& path);
			static constexpr const char* getShaderName(const PxeShaderType type);
			static constexpr const GLenum getShaderGlenum(const PxeShaderType type);

			void initializeGl() override;
			void uninitializeGl() override;
			void bind() override;
			void unbind() override;

		private:
			uint32_t _glProgramId;
			const std::string _path;
			std::unordered_map<std::string, int32_t> _uniformLocations;
		};
	}
}
#endif