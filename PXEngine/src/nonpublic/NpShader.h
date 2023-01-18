#include <stdint.h>
#include <string>
#include <filesystem>
#include <unordered_map>

#include "PxeShader.h"
#include "GL/glew.h"

#ifndef PXENGINE_NONPUBLIC_SHADER_H_
#define PXENGINE_NONPUBLIC_SHADER_H_
namespace pxengine {
	namespace nonpublic {
		class NpShader : public PxeShader
		{
		public:
			//############# PUBLIC API ##################

			int32_t getUniformLocation(const std::string& name) override;
			uint32_t getGlProgramId() const override;
			const std::filesystem::path& getShaderPath() const override;

			void setUniform1fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniform2fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniform3fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniform4fv(const int32_t location, const float* values, uint32_t count) override;

			void setUniform1iv(const int32_t location, const int32_t* values, uint32_t count) override;
			void setUniform2iv(const int32_t location, const int32_t* values, uint32_t count) override;
			void setUniform3iv(const int32_t location, const int32_t* values, uint32_t count) override;
			void setUniform4iv(const int32_t location, const int32_t* values, uint32_t count) override;

			void setUniform1uiv(const int32_t location, const uint32_t* values, uint32_t count) override;
			void setUniform2uiv(const int32_t location, const uint32_t* values, uint32_t count) override;
			void setUniform3uiv(const int32_t location, const uint32_t* values, uint32_t count) override;
			void setUniform4uiv(const int32_t location, const uint32_t* values, uint32_t count) override;

			void setUniformM2fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniformM3fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniformM4fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniformM2x3fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniformM3x2fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniformM2x4fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniformM4x2fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniformM3x4fv(const int32_t location, const float* values, uint32_t count) override;
			void setUniformM4x3fv(const int32_t location, const float* values, uint32_t count) override;

			//############# PRIVATE API ##################

			NpShader(const std::filesystem::path& path);

			virtual ~NpShader();

			// checks the current validation status of the program
			// note only logs on invalid programs and requires a gl context
			bool validateProgram();

		protected:
			enum class PxeShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2, SHADER_COUNT = 3 };
			static uint32_t compileShader(PxeShaderType shaderType, const std::string& source);
			static uint32_t loadShaderFile(const std::filesystem::path& path);
			static constexpr const char* getShaderName(const PxeShaderType type);
			static constexpr const GLenum getShaderGlenum(const PxeShaderType type);

			void initializeGl() override;
			void uninitializeGl() override;
			void bind() override;
			void unbind() override;
			void onDelete() override;

		private:
			uint32_t _glProgramId;
			uint32_t _initializationCount;
			const std::filesystem::path _path;
			std::unordered_map<std::string, int32_t> _uniformLocations;
		};
	}
}
#endif
